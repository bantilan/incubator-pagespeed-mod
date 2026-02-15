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

#include "net/instaweb/rewriter/public/strip_legacy_polyfills_filter.h"

#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/html/html_element.h"
#include "pagespeed/kernel/html/html_name.h"
#include "pagespeed/kernel/html/html_parse.h"

namespace net_instaweb {

StripLegacyPolyfillsFilter::StripLegacyPolyfillsFilter(HtmlParse* html_parse)
    : html_parse_(html_parse) {}

StripLegacyPolyfillsFilter::~StripLegacyPolyfillsFilter() {}

bool StripLegacyPolyfillsFilter::IsLegacyPolyfillUrl(const char* src) const {
  if (src == NULL) {
    return false;
  }

  GoogleString lower_src(src);
  LowerString(&lower_src);

  return (lower_src.find("wp-polyfill") != GoogleString::npos ||
          lower_src.find("polyfill.io") != GoogleString::npos ||
          lower_src.find("/polyfill.min.js") != GoogleString::npos ||
          lower_src.find("/polyfills.min.js") != GoogleString::npos);
}

void StripLegacyPolyfillsFilter::EndElement(HtmlElement* element) {
  if (element->keyword() != HtmlName::kScript) {
    return;
  }
  if (IsLegacyPolyfillUrl(element->AttributeValue(HtmlName::kSrc))) {
    html_parse_->DeleteNode(element);
  }
}

}  // namespace net_instaweb
