# mod_pagespeed
![logo](https://www.gstatic.com/images/branding/product/2x/pagespeed_32dp.png)

|CI|Status|
|---|---|
|Travis|[![Build Status](https://travis-ci.org/apache/incubator-pagespeed-mod.svg?branch=master)](https://travis-ci.org/apache/incubator-pagespeed-mod)|

`mod_pagespeed` is an open-source Apache module created by Google to help Make the Web Faster by rewriting web pages to reduce latency and bandwidth.

## Fork Mission

This fork exists to rebuild and revive mod_pagespeed for modern environments.
The upstream/original project has seen long periods without active maintenance,
while many production stacks still benefit from PageSpeed's optimization model.

Primary objective of this fork:
- Keep PageSpeed usable and buildable on modern toolchains.
- Continue feature updates for current web performance needs.
- Preserve backward compatibility where practical.

### Added in this fork

- AVIF image conversion support (`convert_to_avif`) for modern image delivery.
- LCP-oriented image prioritization filter to improve discoverability and fetch
  priority for above-the-fold/LCP candidates.
- Updated Google Analytics insertion support:
  - Supports modern Google tag IDs (`GT-...`) and GA4 measurement IDs (`G-...`)
    via `gtag.js`.
  - Preserves legacy Universal Analytics behavior for `UA-...` IDs.

### Usage

Nginx example:

```nginx
pagespeed on;
pagespeed FileCachePath /var/cache/ngx_pagespeed;

# AVIF conversion
pagespeed EnableFilters convert_to_avif;

# Prioritize likely LCP images
pagespeed EnableFilters prioritize_lcp_images;

# GA insertion (choose one ID format)
pagespeed EnableFilters insert_ga;
pagespeed AnalyticsID GT-XXXXXXX;   # modern Google tag
# pagespeed AnalyticsID G-XXXXXXX;  # GA4 measurement ID
# pagespeed AnalyticsID UA-XXXXXXX-Y; # legacy Universal Analytics
```

Quick checks:

```bash
# Verify AVIF rewrite path is generated in HTML.
curl -s http://localhost/ | grep -i avif

# Verify rewritten image response is AVIF.
curl -I -H 'Accept: image/avif,image/webp,*/*' \
  'http://localhost/path/to/image.jpg.pagespeed.ic....avif'
```

mod_pagespeed releases are available as [precompiled linux packages](https://modpagespeed.com/doc/download) or as [source](https://modpagespeed.com/doc/build_mod_pagespeed_from_source). (See [Release Notes](https://modpagespeed.com/doc/release_notes) for information about bugs fixed)

mod_pagespeed is an open-source Apache module which automatically applies web performance best practices to pages, and associated assets (CSS, JavaScript, images) without requiring that you modify your existing content or workflow.

mod_pagespeed is built on PageSpeed Optimization Libraries, deployed across 100,000+ web-sites, and provided by popular hosting and CDN providers such as DreamHost, GoDaddy, EdgeCast, and others. There are 40+ available optimizations filters, which include:

- Image optimization, compression, and resizing
- CSS & JavaScript concatenation, minification, and inlining
- Cache extension, domain sharding, and domain rewriting
- Deferred loading of JavaScript and image resources
- and many others...

[![Demo](http://img.youtube.com/vi/8moGR2qf994/0.jpg)](http://www.youtube.com/watch?v=8moGR2qf994)

|  Try it 	|   [modpagespeed.com](https://modpagespeed.com)	|
|---	|---	|
| Slack | https://the-asf.slack.com/archives/CJTG9RH9U |
| Read about it  |https://developers.google.com/speed/pagespeed/module   |
| Download it  | https://modpagespeed.com/doc/download  |
| Check announcements  |https://groups.google.com/group/mod-pagespeed-announce   |
| Discuss it  | https://groups.google.com/group/mod-pagespeed-discuss  |
|FAQ   | https://modpagespeed.com/doc/faq  |


Curious to learn more about mod_pagespeed? Check out our GDL episode below, which covers the history of the project, an architectural overview of how mod_pagespeed works under the hood, and a number of operational tips and best practices for deploying mod_pagespeed.

[![GDL Episode](http://img.youtube.com/vi/6uCAdQSHhmA/0.jpg)](http://www.youtube.com/watch?v=6uCAdQSHhmA)
