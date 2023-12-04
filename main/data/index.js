let websocket = null;

function onMessage(event) {
  var message;
  var switchStatus;
  var JSONContent = JSON.parse(event.data);
  if (JSONContent.hasOwnProperty("state")) {
    if (JSONContent.state == "on") {
      message = "LED ON";
      switchStatus = true;
    } else {
      message = "LED OFF";
      switchStatus = false;
    }
    document.getElementById("LED").innerHTML = message;
    document.getElementById("output").checked = switchStatus;
  }
  if (JSONContent.hasOwnProperty("mode")) {
    document.getElementById("ledmode").value = JSONContent.mode;
    message = "mode:" + JSONContent.mode;
  }
  if (JSONContent.hasOwnProperty("hour")) {
    var hour = parseInt(JSONContent.hour);
    var minute = parseInt(JSONContent.minute);
    if (hour < 10) hour = "0" + hour;
    if (minute < 10) minute = "0" + minute;
    message = hour + ":" + minute;
    document.getElementById("timepicker").value = message;
  }
  if (JSONContent.hasOwnProperty("year")) {
    var month = parseInt(JSONContent.month);
    var date = parseInt(JSONContent.date);
    if (month < 10) month = "0" + month;
    if (date < 10) date = "0" + date;
    message = JSONContent.year + "-" + month + "-" + date;
    document.getElementById("datepicker").value = message;
  }
}

// Swtich click send data
function switchLED(element) {
  var command;
  if (element.checked) command = '{"state":"on"}';
  else command = '{"state":"off"}';
  if (websocket) websocket.send(command);
}

// Select led mode
function selectLEDMode() {
  var modeValue = document.getElementById("ledmode").value;
  modeValue = '{"mode":' + modeValue + "}";
  if (websocket) websocket.send(modeValue);
}

/* Set time */
function setTime() {
  if (document.getElementById("ledmode").value != 0) {
    alert("该功能只在显示日期模式下有效！");
    return;
  }
  var time = document.getElementById("timepicker").value;
  time = '{"hour":' + parseInt(time.substr(0, 2)) + ',"minute":' + parseInt(time.substr(3, 2)) + "}";
  if (websocket) websocket.send(time);
}

/* Set date */
function setDate() {
  if (document.getElementById("ledmode").value != 1) {
    alert("该功能只在显示日期模式下有效！");
    return;
  }
  var date = document.getElementById("datepicker").value;
  date =
    '{"year":' +
    date.substr(0, 4) +
    ',"month":' +
    parseInt(date.substr(5, 2)) +
    ',"date":' +
    parseInt(date.substr(8, 2)) +
    "}";
  if (websocket) websocket.send(date);
}

function handleConnect() {
  websocket = new WebSocket(`ws://${window.location.hostname}/ws`);

  websocket.onmessage = onMessage;
  websocket.onclose = () => setTimeout(handleConnect, 1000);
}

handleConnect();
