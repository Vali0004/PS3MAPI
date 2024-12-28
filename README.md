# PS3MAPI
### A C++ Client implementation of PS3MAPI as a library with C-Style bindings avaliable
## Why?
#### Well, mainly because I wanted a easier way to patch games. However, there are several other reasons why.
#### Reason 1:
  * CCAPI does not have full DEX support (4.83 and anything past 4.84)
  * Not everyone has CCAPI installed
  * TMAPI is extremely unreliable and doesn't always work
  * PS3MAPI seems to have more features than its alternatives
## How can I use this?
#### Right now? You can build the project, include c_bindings, and link PS3MAPI.lib. There is a example project on how.
#### In the future? I'd like to make it a dynamic library, so it can be used in C# and LuaFFI.
