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

#include "net/instaweb/rewriter/public/rewrite_options.h"
#include "net/instaweb/rewriter/public/rewrite_test_base.h"
#include "pagespeed/kernel/base/gtest.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_writer.h"

namespace net_instaweb {

class PrioritizeLcpImagesFilterTest : public RewriteTestBase {
 public:
  PrioritizeLcpImagesFilterTest() : writer_(&output_) {}

 protected:
  void SetUp() override {
    options()->EnableFilter(RewriteOptions::kPrioritizeLcpImages);
    RewriteTestBase::SetUp();
    rewrite_driver()->AddFilters();
    rewrite_driver()->SetWriter(&writer_);
  }

  GoogleString output_;

 private:
  StringWriter writer_;
};

TEST_F(PrioritizeLcpImagesFilterTest, PrioritizesFirstImage) {
  const GoogleString input =
      "<head></head><body>"
      "<img loading=\"lazy\" src=\"/hero.jpg\">"
      "<img loading=\"lazy\" src=\"/next.jpg\">"
      "</body>";
  const GoogleString expected =
      "<head></head><body>"
      "<link rel=\"preload\" as=\"image\" href=\"/hero.jpg\"/>"
      "<img src=\"/hero.jpg\" fetchpriority=\"high\" "
      "data-pagespeed-no-defer=\"1\" pagespeed_no_defer=\"1\"/>"
      "<img loading=\"lazy\" src=\"/next.jpg\"/>"
      "</body>";
  Parse("prioritize_first_image", input);
  EXPECT_EQ(AddHtmlBody(expected), output_);
}

TEST_F(PrioritizeLcpImagesFilterTest, PrioritizesImgInsidePicture) {
  const GoogleString input =
      "<head></head><body>"
      "<picture><source srcset=\"/hero.webp\"><img src=\"/hero.jpg\" "
      "loading=\"lazy\"></picture>"
      "</body>";
  const GoogleString expected =
      "<head></head><body>"
      "<picture><source srcset=\"/hero.webp\"/>"
      "<link rel=\"preload\" as=\"image\" href=\"/hero.jpg\"/>"
      "<img src=\"/hero.jpg\" fetchpriority=\"high\" "
      "data-pagespeed-no-defer=\"1\" pagespeed_no_defer=\"1\"/>"
      "</picture>"
      "</body>";
  Parse("prioritize_picture_img", input);
  EXPECT_EQ(AddHtmlBody(expected), output_);
}

}  // namespace net_instaweb
