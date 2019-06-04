# node-wifi-info

## What is this?
The `node-wifi-info` module allows macOS, Windows, and Linux<sup>1</sup> users to get information about the currently connected wifi including SSID, BSSID, signalStrength(dBm) and security status.

<sup>1</sup> Linux support right now is limited. Although full feature is provided, the underlying implementation is quite slow. Thus the performance is not comparable to the windows and macOS module.

## FAQ
### Why write this?
During the process of locating this package, you might have stumbled upon the `node-wifi` module. Why did I build this when there is a package already available? Although it did provide all the feature I wanted, there are a few things that could be improved: namely the underlying implementation is not stable and slow. Especially for Windows and macOS. As it directly invoking system commands and parsing the output from it; however, as those command-line tools are designed for end user, not for programs, the output format could change without notice and rendering the module broken. 

Although this module is significantly faster on macOS and Windows, due to the use of native APIs from respective operating system. There are definitely certain advantages that node-wifi has over this, namely not having any native modules thus reducing chances of running into problems when you need to cross-compile

## Performance compared to node-wifi
*Only Windows and macOS performace is compared, as our underlying Linux implementation is not siginificantly different from* `node-wifi`.

*The following results are averaged from 1000 invocations at random intervals between 0-10 seconds*

| fetch connected wifi info (average invocation time) | Windows | macOS   |
|-----------------------------------------------------|---------|---------|
| node-wifi                                           | 69.99ms | 16.58ms |
| node-wifi-info (this module)                        | 0.35ms  | 2.9ms   |
| improvement                                         | ~200x   | ~5.7x   |

# License
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this software except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

# Contribution
Pull Requests are of course welcome! Feel free to help implementing the Linux native API. Feel free to submit bug reports and any issue you find while using this module. Any sugesstions on the Linux implementation would be greatly appreciated. 