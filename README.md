# Hydra

[![](http://7xqoxy.com1.z0.glb.clouddn.com/npm-passing.svg)](https://github.com/Kar365/hydra "")
[![](http://7xqoxy.com1.z0.glb.clouddn.com/node-version.svg)](https://github.com/Kar365/hydra "")
[![](http://7xqoxy.com1.z0.glb.clouddn.com/lisence-bsd.svg)](https://github.com/Kar365/hydra "")

![Hydra](http://7xqoxy.com1.z0.glb.clouddn.com/hydra1835.jpg)

hydra 是一个 node.js 的 C++ 模块，用来实现将复杂的计算任务委托给其他线程执行。

在 Js 代码中创建线程对象，执行 CPU 密集型函数，例如计算文件 HASH，加密解密等任务。在多核环境下，线程对象有助于更好的利用多核优势，以可控的方式减缓主线程压力。

This is a package for providing ability to create new threads in js. It helps you consciously assign cpu-bound tasks to a limited number of CPUs.

## Platform Support
> Windows, Linux and OS X

## Node engine
> \>=4.0.0

## Compile & Link

```
先全局安装 node-gyp
在工程根目录下运行 node-gyp configure
在工程根目录下运行 node-gyp rebuild
（编译过程大概需要 10s 左右，如果出现提示无权访问目录，请尝试 sudo node-gyp rebuild）

npm install -g node-gyp
node-gyp configure
node-gyp build (or  **sudo node-gyp rebuild** )
```

## Examples

**将一个定时器抛给线程对象，等待2秒钟后，回到主线程执行回调函数。**

```js
var Thread = require('hydra')
var thread = new Thread()

thread.delayBySec(2, function(err) {
  if(err) {
    console.error(err)
  }

  console.log('after two secs')
  thread.close()
  console.log('thread running state: ' + thread.isRunning())
  thread = null
  })

console.log('thread running state: ' + thread.isRunning())
```

**Compute a file hash in other threads**

```js
var path = require('path')
var fs = require('fs')
var assert = require('assert')
var Thread = require('hydra')
var thread = new Thread()

console.log('before compute')
fs.readFile('./test.tmp', function(err, data) {
  thread.sha2({data, type: 256}, function(err, data) {
    if(err) {
      return console.error(err)
    }
    console.log('result:')
    console.log(data)
    console.log('after compute')
    console.log('The number of tasks queued：' + thread.numOfTasks())
  })
  console.log('The number of tasks queued：' + thread.numOfTasks())
})
```

**Promise**

```js
var path = require('path')
var fs = require('fs')
var assert = require('assert')
var Thread = require('hydra')
const Promise = require('bluebird')
const co = Promise.coroutine

var thread = new Thread()

fs.readFile('./test/thread.js', function(err, data) {
  co(function*() {
    var r = yield thread.sha2({data, type: 256})
    console.log(r)
  })()
})

```

**Ed25519 signature(EdDSA)**

```js
const Thread = require('hydra')
var thread = new Thread()

// async
thread.sign(new Buffer('a message'),
  new Buffer('af9881fe34edfd3463cf3e14e22ad95a0608967e084d3ca1fc57be023040de59'),
  function(err, data) {
    console.log(data.toString('hex'))
  })

// sync, compute in main thread
var data = Thread.sign(new Buffer('a message'),
  new Buffer('af9881fe34edfd3463cf3e14e22ad95a0608967e084d3ca1fc57be023040de59'))
```

## APIs

```
close  // 关闭线程
isRunning  // 返回线程对象的线程是否运行(存在)
numOfTasks  // 线程队列里CPU密集型任务个数
sha2  // SHA {256, 384, 512}
makeKeypair // 使用 Ed25519 生成密钥对
sign // Ed25519-DSA sign
verify // Ed25519 verify
```

## Module dependency

## Testing dependency

## More descriptions

[Ed25519 implementation](https://github.com/dazoe/ed25519)

## License

BSD
