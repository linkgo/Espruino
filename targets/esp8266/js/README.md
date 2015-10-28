env.js
======

Just a js for test.

How to Apply
============

[node-espruino](https://www.npmjs.com/package/node-espruino)
Need patching:
`/usr/lib/node_modules/node-espruino/node-espruino.js: +276`
```
var builtin_modules = ["http", "fs", "CC3000", "net", "wifi", "ESP8266"]; // espruino has these modules built in, ignore them.
```
Also, there may be more builtin_modules.

To upload
```
$ espruino upload env.js --port /dev/ttyUSB1 --save
```

Use Cases
=========

Soft AP Mode
------------

```
> wifi.createAP("espruino-ap");
```


Serve a Web
-----------

```
> http.createServer(onPage).listen(80);
```
