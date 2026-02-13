/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "net/instaweb/rewriter/public/prioritize_lcp_images_filter.h"

#include "net/instaweb/rewriter/public/rewrite_driver.h"
#include "net/instaweb/rewriter/public/rewrite_options.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/html/html_element.h"
#include "pagespeed/kernel/html/html_name.h"
#include "pagespeed/kernel/http/data_url.h"
#include "pagespeed/kernel/http/google_url.h"
#include "pagespeed/opt/logging/enums.pb.h"
#include "pagespeed/opt/logging/log_record.h"

namespace {

const int kMaxPrioritizedImages = 1;

}  // namespace

namespace net_instaweb {

PrioritizeLcpImagesFilter::PrioritizeLcpImagesFilter(RewriteDriver* driver)
    : CommonFilter(driver) {
  StartDocumentImpl();
}

PrioritizeLcpImagesFilter::~PrioritizeLcpImagesFilter() {}

void PrioritizeLcpImagesFilter::DetermineEnabled(GoogleString* disabled_reason) {
  set_is_enabled(true);
}

void PrioritizeLcpImagesFilter::StartDocumentImpl() {
  in_head_ = false;
  in_body_ = false;
  prioritized_count_ = 0;
  preloaded_hrefs_.clear();
}

void PrioritizeLcpImagesFilter::StartElementImpl(HtmlElement* element) {
  if (element->keyword() == HtmlName::kHead) {
    in_head_ = true;
    return;
  }
  if (element->keyword() == HtmlName::kBody) {
    in_body_ = true;
    return;
  }
  if (noscript_element() != NULL || prioritized_count_ >= kMaxPrioritizedImages
      || !in_body_) {
    return;
  }
  if (element->keyword() != HtmlName::kImg || !driver()->IsRewritable(element)) {
    return;
  }

  GoogleString src;
  if (!IsEligibleImage(element, &src)) {
    return;
  }
  PrioritizeImage(element, src);
}

void PrioritizeLcpImagesFilter::EndElementImpl(HtmlElement* element) {
  if (element->keyword() == HtmlName::kHead) {
    in_head_ = false;
  } else if (element->keyword() == HtmlName::kBody) {
    in_body_ = false;
  }
}

bool PrioritizeLcpImagesFilter::IsHiddenByStyle(HtmlElement* element) const {
  HtmlElement::Attribute* style_attr = element->FindAttribute(HtmlName::kStyle);
  if (style_attr == NULL || style_attr->DecodedValueOrNull() == NULL) {
    return false;
  }
  GoogleString style = style_attr->DecodedValueOrNull();
  LowerString(&style);
  return (style.find("display:none") != GoogleString::npos ||
          style.find("visibility:hidden") != GoogleString::npos);
}

bool PrioritizeLcpImagesFilter::LooksLikeTinyTracker(HtmlElement* element) const {
  int width = 0;
  int height = 0;
  const char* width_str = element->AttributeValue(HtmlName::kWidth);
  const char* height_str = element->AttributeValue(HtmlName::kHeight);
  if (width_str != NULL && height_str != NULL &&
      StringToInt(width_str, &width) && StringToInt(height_str, &height)) {
    return width <= 1 && height <= 1;
  }
  return false;
}

bool PrioritizeLcpImagesFilter::IsEligibleImage(
    HtmlElement* element, GoogleString* src_out) const {
  if (IsHiddenByStyle(element) || LooksLikeTinyTracker(element)) {
    return false;
  }
  HtmlElement::Attribute* src_attr = element->FindAttribute(HtmlName::kSrc);
  if (src_attr == NULL || src_attr->DecodedValueOrNull() == NULL) {
    return false;
  }

  StringPiece src(src_attr->DecodedValueOrNull());
  if (src.empty() || IsDataUrl(src)) {
    return false;
  }

  GoogleUrl gurl(driver()->base_url(), src);
  if (!gurl.IsAnyValid()) {
    return false;
  }
  src.CopyToString(src_out);
  return true;
}

void PrioritizeLcpImagesFilter::PrioritizeImage(
    HtmlElement* element, const GoogleString& src) {
  // Ensure this candidate is not lazy/deferred by browser or other filters.
  HtmlElement::Attribute* loading_attr = element->FindAttribute("loading");
  if (loading_attr != NULL &&
      loading_attr->DecodedValueOrNull() != NULL &&
      StringCaseEqual(loading_attr->DecodedValueOrNull(), "lazy")) {
    element->DeleteAttribute("loading");
  }

  HtmlElement::Attribute* fetchpriority_attr =
      element->FindAttribute("fetchpriority");
  if (fetchpriority_attr != NULL) {
    fetchpriority_attr->SetValue("high");
  } else {
    element->AddAttribute(HtmlName::kFetchpriority, "high",
                          HtmlElement::DOUBLE_QUOTE);
  }

  if (element->FindAttribute(HtmlName::kDataPagespeedNoDefer) == NULL) {
    element->AddAttribute(HtmlName::kDataPagespeedNoDefer, "1",
                          HtmlElement::DOUBLE_QUOTE);
  }
  if (element->FindAttribute(HtmlName::kPagespeedNoDefer) == NULL) {
    element->AddAttribute(HtmlName::kPagespeedNoDefer, "1",
                          HtmlElement::DOUBLE_QUOTE);
  }

  // Insert preload right before the prioritized image so discovery happens in
  // the initial HTML document parse.
  if (preloaded_hrefs_.insert(src).second) {
    HtmlElement* preload = driver()->NewElement(element, HtmlName::kLink);
    driver()->AddAttribute(preload, HtmlName::kRel, "preload");
    driver()->AddAttribute(preload, HtmlName::kAs, "image");
    driver()->AddAttribute(preload, HtmlName::kHref, src);
    driver()->InsertNodeBeforeNode(element, preload);
  }

  ++prioritized_count_;
  driver()->log_record()->SetRewriterLoggingStatus(
      RewriteOptions::FilterId(RewriteOptions::kPrioritizeLcpImages),
      RewriterApplication::APPLIED_OK);
}

}  // namespace net_instaweb
