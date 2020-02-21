const nodeWifiInfo = require('../node_wifi_info');

async function sleep(millis) {
  return new Promise(resolve => setTimeout(resolve, millis));
}

async function main() {
  var max = 0;
  var sum = 0;
  var data = [];
  const limit = 1000;

  for (var i = 0; i < limit; i++) {
    const start = new Date().getTime();
    const rtn = nodeWifiInfo.getWifiInfo();
    const end = new Date().getTime();
    const delta = end-start;

    console.log(rtn);

    sum += delta;
    if (delta > max) {
      max = delta;
    }

    console.log("time =", delta);
    
    data.push(delta);
    await sleep(Math.random() * 1000);
  }

  console.log("average", sum / limit);
  console.log("max", max);

  let obj = {};
  for (let i = 0; i < data.length; i++) {
    if (obj[data[i]] == undefined) {
      obj[data[i]] = 1;
    } else {
      obj[data[i]] += 1;
    }
  }

  console.log(obj);
}

main();
