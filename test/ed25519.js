const assert = require('assert');
const Thread = require('../index.js');
const Promise = require('bluebird');
const co = Promise.coroutine;
const naclFactory = require('js-nacl')
const nacl = {}
const crypto = require('crypto')

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

var thread = new Thread()

var data = {
  seed: 'af9881fe34edfd3463cf3e14e22ad95a0608967e084d3ca1fc57be023040de59',
  privateKey:'af9881fe34edfd3463cf3e14e22ad95a0608967e084d3ca1fc57be023040de590c32c468980d40237f4e44a66dec3beb564b3e1394a4c6df1da2065e3afc1d81',
  publicKey: '0c32c468980d40237f4e44a66dec3beb564b3e1394a4c6df1da2065e3afc1d81',
  message: 'test',
  signature: '98c8351675ade54b3aedc14f0b9c40b47569d9da191db066312ed6423d20dff8a52988f869fc3fbf4402971034b387ac7fbcfa704eb4c1e86e48e15de5e3d206',
  invalidSignature: '88c8351675ade54b3aedc14f0b9c40b47569d9da191db066312ed6423d20dff8a52988f869fc3fbf4402971034b387ac7fbcfa704eb4c1e86e48e15de5e3d205'
};

describe('#ed25519', function() {
  describe('makeKeypair()', function () {
    it('returns a public and private key', function () {
      var seed = new Buffer(data.seed, 'hex');
      var keyPair = Thread.makeKeypair(seed);
      assert.equal(keyPair.publicKey.toString('hex'), data.publicKey)
      assert.equal(keyPair.privateKey.toString('hex'), data.privateKey)
    })

    it('call signSync then verifySync', function () {
      var seed = new Buffer(data.seed, 'hex');
      var keyPair = Thread.makeKeypair(seed);
      var message = new Buffer(data.message);
      const signature = Thread.sign(message, keyPair)
      assert(Thread.verify(message, signature, keyPair.publicKey))
    })

  })

  describe('sign()', function () {
    it('Generates a valid signature using a seed', function(done) {
      var seed = new Buffer(data.seed, 'hex');
      var message = new Buffer(data.message);
      thread.sign(message, seed, function(err, signature) {
        if (err) console.error(err);
        assert.equal(signature.toString('hex'), data.signature);
        done();
      });
    });

    it('Generates a valid signature using a keyPair', function(done) {
      var privateKey = new Buffer(data.privateKey, 'hex');
      var publicKey = new Buffer(data.publicKey, 'hex');
      var message = new Buffer(data.message);
      thread.sign(message, { privateKey, publicKey },
        function(err, signature) {
          assert.equal(signature.toString('hex'), data.signature);
          done();
        }
      );
    });
  });

  describe('yield sign', function () {
    it('Generates a valid signature using a seed', function() {
      return co(function*() {
        var seed = new Buffer(data.seed, 'hex');
        var message = new Buffer(data.message);
        var signature = yield thread.sign(message, seed);
        assert.equal(signature.toString('hex'), data.signature);
      })();
    });

    it('Generates a valid signature using a keyPair', function() {
      return co(function*() {
        var privateKey = new Buffer(data.privateKey, 'hex');
        var publicKey = new Buffer(data.publicKey, 'hex');
        var message = new Buffer(data.message);
        var signature = yield thread.sign(message, {privateKey, publicKey});
        assert.equal(signature.toString('hex'), data.signature);
      })();
    });
  });

  describe('verify', function() {
    it('returns true if the signature is valid', function(done) {
      var publicKey = new Buffer(data.publicKey, 'hex');
      var signature = new Buffer(data.signature, 'hex');
      var message = new Buffer(data.message);
      thread.verify(message, signature, publicKey, function(err, Is) {
        assert.ok(Is);
        done();
      });
    });

    it('returns false if the signature is not valid', function(done) {
      var publicKey = new Buffer(data.publicKey, 'hex');
      var signature = new Buffer(data.invalidSignature, 'hex');
      var message = new Buffer(data.message);
      thread.verify(message, signature, publicKey, function(err, Is) {
        assert.ifError(Is);
        done();
      });
    });
  });

  describe('yield verify', function() {
    it('returns true if the signature is valid', function() {
      return co(function* () {
        var publicKey = new Buffer(data.publicKey, 'hex');
        var signature = new Buffer(data.signature, 'hex');
        var message = new Buffer(data.message);
        var r = yield thread.verify(message, signature, publicKey);
        assert.ok(r);
      })()
    })

    it('returns false if the signature is not valid', function() {
      return co(function* () {
        var publicKey = new Buffer(data.publicKey, 'hex');
        var signature = new Buffer(data.invalidSignature, 'hex');
        var message = new Buffer(data.message);
        var r = yield thread.verify(message, signature, publicKey);
        assert.ifError(r);
      })()
    })
    it('compare with js-nacl', function() {
      return co(function* () {
        const randstr = 'fdsa788af98asfsadfsaufgdsa7tfsadgf8fdsaf7d6fdfsgyuddusafy8sa7fy'
        const hash = crypto.createHash('sha256').update(randstr).digest();
        const naclPair = nacl.makeKeypair(hash)
        const thisPair = Thread.makeKeypair(hash)
        assert(Buffer.isBuffer(thisPair.privateKey) && Buffer.isBuffer(thisPair.publicKey))
        assert(naclPair.privateKey.toString('hex') === thisPair.privateKey.toString('hex'))
        assert(naclPair.publicKey.toString('hex') === thisPair.publicKey.toString('hex'))
        const signature = yield thread.sign(hash, thisPair)
        assert(Buffer.isBuffer(signature))
        assert(yield thread.verify(hash, signature, thisPair.publicKey))
        assert(nacl.verify(hash, signature, thisPair.publicKey))
      })()
    })
    it('sha256', function () {
      return co(function* () {
        var randstr = Buffer.from('dsfafdsdfsafsa88sdaf8dsf89dsa8fdsa898fdsa8f89sa')
        // randstr = Buffer.from([0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0, 809897])
        const hash = crypto.createHash('sha256').update(randstr).digest()
        // console.log(hash.toString())
        const thisHash = yield thread.sha2({data: randstr})
        // console.log(thisHash.toString())
        assert(Buffer.isBuffer(thisHash))
        assert(hash.toString('hex') === thisHash.toString('hex'))
      })()
    })
  })
})
