var websocket = new WebSocket(`ws://${window.location.hostname}/ws`);

// websocket event declear
websocket.onopen = function (event) {
  onOpen(event);
};
websocket.onclose = function (event) {
  onClose(event);
};
websocket.onerror = function (event) {
  onError(event);
};
websocket.onmessage = function (event) {
  onMessage(event);
};

// websocket events
function onOpen(event) {
  console.log("Server Connected!");
}
function onClose(event) {
  console.log("Server Disconnected!");
  alert("Server Disconnected!");
}
function onError(event) {
  console.log("Error:" + event.data);
  alert("Error Occured!");
}
function onMessage(event) {
  var message;
  var switchStatus;
  var JSONContent = JSON.parse(event.data);
  if (JSONContent.hasOwnProperty("LED")) {
    if (JSONContent.LED == "ON") {
      message = "LED ON";
      switchStatus = true;
    } else {
      message = "LED OFF";
      switchStatus = false;
    }
    document.getElementById("LED").innerHTML = message;
    document.getElementById("output").checked = switchStatus;
  }
  if (JSONContent.hasOwnProperty("Mode")) {
    document.getElementById("ledmode").value = JSONContent.Mode;
    message = "Mode:" + JSONContent.Mode;
  }
  if (JSONContent.hasOwnProperty("Hour")) {
    var hour = parseInt(JSONContent.Hour);
    var minute = parseInt(JSONContent.Minute);
    if (hour < 10) hour = "0" + hour;
    if (minute < 10) minute = "0" + minute;
    message = hour + ":" + minute;
    document.getElementById("timepicker").value = message;
  }
  if (JSONContent.hasOwnProperty("Year")) {
    var month = parseInt(JSONContent.Month);
    var date = parseInt(JSONContent.Date);
    if (month < 10) month = "0" + month;
    if (date < 10) date = "0" + date;
    message = JSONContent.Year + "-" + month + "-" + date;
    document.getElementById("datepicker").value = message;
  }
}

// Swtich click send data
function switchLED(element) {
  var command;
  if (element.checked) command = '{"LED":"ON"}';
  else command = '{"LED":"OFF"}';
  websocket.send(command);
  console.log(command);
}

// Select led mode
function selectLEDMode(element) {
  var modeValue = document.getElementById("ledmode").value;
  modeValue = '{"Mode":' + modeValue + "}";
  console.log(modeValue);
  websocket.send(modeValue);
}

/* Set time */
function setTime(element) {
  if (document.getElementById("ledmode").value != 0) {
    alert("该功能只在显示日期模式下有效！");
    return;
  }
  var time = document.getElementById("timepicker").value;
  time = '{"Hour":' + parseInt(time.substr(0, 2)) + ',"Minute":' + parseInt(time.substr(3, 2)) + "}";
  console.log(time);
  websocket.send(time);
}

/* Set date */
function setDate(element) {
  if (document.getElementById("ledmode").value != 1) {
    alert("该功能只在显示日期模式下有效！");
    return;
  }
  var date = document.getElementById("datepicker").value;
  date =
    '{"Year":' +
    date.substr(0, 4) +
    ',"Month":' +
    parseInt(date.substr(5, 2)) +
    ',"Date":' +
    parseInt(date.substr(8, 2)) +
    "}";
  console.log(date);
  websocket.send(date);
}
