const fs = require('fs')
const Thread = require('../index.js')
const Promise = require('bluebird')
const co = Promise.coroutine

const thread = new Thread()

fs.readFile('./mem-pressure-test', function(err, data) {
  co(function*() {
    for (;;) {
      // var r =
      yield thread.sha2({data, type: 256})
    }
  })().catch(function(e) {
    console.error(e)
  })
})
