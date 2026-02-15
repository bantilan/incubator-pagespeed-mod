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

#include "pagespeed/kernel/base/basictypes.h"
#include "pagespeed/kernel/base/gtest.h"
#include "pagespeed/kernel/html/html_parse.h"
#include "pagespeed/kernel/html/html_parse_test_base.h"

namespace net_instaweb {

class StripLegacyPolyfillsFilterTest : public HtmlParseTestBase {
 protected:
  StripLegacyPolyfillsFilterTest()
      : strip_legacy_polyfills_filter_(&html_parse_) {
    html_parse_.AddFilter(&strip_legacy_polyfills_filter_);
  }

 private:
  StripLegacyPolyfillsFilter strip_legacy_polyfills_filter_;

  DISALLOW_COPY_AND_ASSIGN(StripLegacyPolyfillsFilterTest);
};

TEST_F(StripLegacyPolyfillsFilterTest, RemovesWordpressPolyfill) {
  ValidateExpected(
      "remove_wp_polyfill",
      "<head><script type='text/javascript' "
      "src='https://c0.wp.com/c/5.8.12/wp-includes/js/dist/vendor/"
      "wp-polyfill.min.js' id='wp-polyfill-js'></script></head>"
      "<body>Hello</body>",
      "<head></head><body>Hello</body>");
}

TEST_F(StripLegacyPolyfillsFilterTest, RemovesPolyfillIo) {
  ValidateExpected(
      "remove_polyfill_io",
      "<head><script src='https://polyfill.io/v3/polyfill.min.js?features="
      "default'></script></head><body>Hello</body>",
      "<head></head><body>Hello</body>");
}

TEST_F(StripLegacyPolyfillsFilterTest, KeepsRegularScriptsAndInlineCode) {
  ValidateExpected(
      "keep_normal_scripts",
      "<head><script src='/app.js'></script><script>var x = 1;</script></head>"
      "<body>Hello</body>",
      "<head><script src=\"/app.js\"></script><script>var x = 1;</script></head>"
      "<body>Hello</body>");
}

}  // namespace net_instaweb
