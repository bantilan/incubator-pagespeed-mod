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

#ifndef NET_INSTAWEB_REWRITER_PUBLIC_PRIORITIZE_LCP_IMAGES_FILTER_H_
#define NET_INSTAWEB_REWRITER_PUBLIC_PRIORITIZE_LCP_IMAGES_FILTER_H_

#include <set>

#include "net/instaweb/rewriter/public/common_filter.h"
#include "pagespeed/kernel/base/basictypes.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/html/html_element.h"

namespace net_instaweb {

// Rewriter that heuristically prioritizes an early in-viewport image:
// 1) removes loading="lazy"
// 2) adds fetchpriority="high"
// 3) inserts <link rel="preload" as="image" href="..."> before the image
// 4) adds pagespeed_no_defer/data-pagespeed-no-defer to avoid defer filters
class PrioritizeLcpImagesFilter : public CommonFilter {
 public:
  explicit PrioritizeLcpImagesFilter(RewriteDriver* driver);
  ~PrioritizeLcpImagesFilter() override;

  const char* Name() const override { return "PrioritizeLcpImages"; }

 private:
  void DetermineEnabled(GoogleString* disabled_reason) override;
  void StartDocumentImpl() override;
  void StartElementImpl(HtmlElement* element) override;
  void EndElementImpl(HtmlElement* element) override;

  bool IsEligibleImage(HtmlElement* element, GoogleString* src_out) const;
  void PrioritizeImage(HtmlElement* element, const GoogleString& src);
  bool IsHiddenByStyle(HtmlElement* element) const;
  bool LooksLikeTinyTracker(HtmlElement* element) const;

  bool in_head_;
  bool in_body_;
  int prioritized_count_;
  std::set<GoogleString> preloaded_hrefs_;

  DISALLOW_COPY_AND_ASSIGN(PrioritizeLcpImagesFilter);
};

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_REWRITER_PUBLIC_PRIORITIZE_LCP_IMAGES_FILTER_H_
