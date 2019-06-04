var myModule

if (process.env.DEBUG) {
  myModule = require('./build/Debug/node-wifi-info.node')
} else {
  myModule = require('./build/Release/node-wifi-info.node')
}

module.exports = myModule
