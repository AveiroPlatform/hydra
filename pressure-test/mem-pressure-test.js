const Thread = require('../index.js')
const assert = require('assert')
const crypto = require('crypto')
const randomstring = require('randomstring')
const Promise = require('bluebird')
const co = Promise.coroutine

const naclFactory = require('js-nacl')
const nacl = {}

naclFactory.instantiate((naclInstance) => {
  nacl.makeKeypair = (hash) => {
    const keyPair = naclInstance.crypto_sign_keypair_from_seed(hash)
    return {
      publicKey: new Buffer(keyPair.signPk),
      privateKey: new Buffer(keyPair.signSk),
    }
  }

  nacl.sign = (hash, keyPair) => {
    const signature = naclInstance.crypto_sign_detached(hash, new Buffer(keyPair.privateKey, 'hex'))
    return new Buffer(signature)
  }

  nacl.verify = (hash, signature, publicKey) => {
    return naclInstance.crypto_sign_verify_detached(new Buffer(signature, 'hex'),
      hash, new Buffer(publicKey, 'hex'))
  }
})

module.exports = nacl

co(function*() {
  var loop = 0
  var count = 0
  var rand
  var hash
  var seed
  var keyPair
  for (;;) {
    ++loop
    ++count
    if (loop > 100000) {
      yield Promise.delay(1000)
      loop = 0
    }
    rand = randomstring.generate({ length: 64 })
    hash = crypto.createHash('sha256').update(rand).digest()
    seed = new Buffer(hash, 'utf8')
    keyPair = Thread.makeKeypair(seed)
    rand = randomstring.generate({ length: 120 })
    hash = crypto.createHash('sha256').update(rand).digest()
    const signature = Thread.sign(hash, keyPair)
    assert(Thread.verify(hash, signature, keyPair.publicKey))
    // assert(nacl.verify(hash, signature, keyPair.publicKey))
    if (loop % 1000 === 0) {
      console.log(count)
    }
  }
})().catch(function(e) {
  console.error(e)
})
