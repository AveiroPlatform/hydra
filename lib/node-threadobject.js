const rcib = require('../build/Release/node-threadobject.node')
const THREAD = rcib.THREAD

function Thread() {
  const o = {
    close() {
      thread_.close();
    },
    isRunning() {
      return thread_.isRunning();
    },
    delayByMil(mils, cb) {
      thread_.delayByMil(mils, function(err) {
        setImmediate(() => cb(err))
      })
    },
    delayBySec(secs, cb) {
      thread_.delayBySec(secs, function(err) {
        setImmediate(() => cb(err))
      })
    },
    delayByMin(mins, cb) {
      thread_.delayByMin(mins, function(err) {
        setImmediate(() => cb(err))
      })
    },
    delayByHour(hours, cb) {
      thread_.delayByHour(hours, function(err) {
        setImmediate(() => cb(err))
      })
    },
    numOfTasks() {
      return thread_.queNum()
    },
    sign(hash, Key, cb) {
      if (!Buffer.isBuffer(hash)) {
        hash = Buffer.from(hash, 'hex')
      }
      thread_.sign(hash, Key, function(err, rets) {
        setImmediate(() => cb(err, rets))
      })
    },
    verify(hash, signature, pKey, cb) {
      if (!Buffer.isBuffer(hash)) {
        hash = Buffer.from(hash, 'hex')
      }
      if (!Buffer.isBuffer(signature)) {
        signature = Buffer.from(signature, 'hex')
      }
      if (!Buffer.isBuffer(pKey)) {
        pKey = Buffer.from(pKey, 'hex')
      }
      thread_.verify(hash, signature, pKey, function(err, rets) {
        setImmediate(() => cb(err, rets));
      })
    },
    sha2(param, cb) {
      const type = param.type ? param.type : 256
      const data = Buffer.isBuffer(param.data) ? param.data : Buffer.from(param.data, 'utf8')
      if (256 === type || 384 === type || 512 === type) {
        thread_.sha2(type, data, function(err, rets) {
          setImmediate(() => cb(err, rets))
        });
      } else {
        setImmediate(() => cb(new Error('type should be one of {256,384,512}')))
      }
    }
  }

  const thread_ =  new THREAD()
  Promise.promisifyAll(o, {suffix: 'Async'})

  return {
    close() {
      o.close()
    },
    isRunning() {
      return o.isRunning()
    },
    delayByMil(mils, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.delayByMil(mils, cb);
      } else {
        return o.delayByMilAsync(mils);
      }
    },
    delayBySec(secs, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.delayBySec(secs, cb);
      } else {
        return o.delayBySecAsync(secs);
      }
    },
    delayByMin(mins, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.delayByMin(mins, cb);
      } else {
        return o.delayByMinAsync(mins);
      }
    },
    delayByHour(hours, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.delayByHour(hours, cb);
      } else {
        return o.delayByHourAsync(hours);
      }
    },
    numOfTasks() {
      return o.numOfTasks()
    },
    sign(message, Key, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.sign(message, Key, cb)
      } else {
        return o.signAsync(message, Key)
      }
    },
    verify(message, signature, pKey, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.verify(message, signature, pKey, cb)
      } else {
        return o.verifyAsync(message, signature, pKey)
      }
    },
    sha2(param, cb) {
      if (cb && typeof cb === 'function' && cb.constructor.name === 'Function') {
        o.sha2(param, cb)
      } else {
        return o.sha2Async(param)
      }
    }
  }
}

Thread.makeKeypair = (seed) => {
  if (!Buffer.isBuffer(seed)) {
    seed = Buffer.from(seed, 'hex')
  }
  return rcib.makeKeypair(seed)
}

Thread.sign = (hash, Key) => {
  if (!Buffer.isBuffer(hash)) {
    hash = Buffer.from(hash, 'hex')
  }
  return rcib.signSync(hash, Key)
}

Thread.verify = (hash, signature, pKey) => {
  if (!Buffer.isBuffer(hash)) {
    hash = Buffer.from(hash, 'hex')
  }
  if (!Buffer.isBuffer(signature)) {
    signature = Buffer.from(signature, 'hex')
  }
  if (!Buffer.isBuffer(pKey)) {
    pKey = Buffer.from(pKey, 'hex')
  }
  return rcib.verifySync(hash, signature, pKey)
}

module.exports = Thread
