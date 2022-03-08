var websocket = new WebSocket("ws://" + location.hostname + ":81/");

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
  alert("Server Connected!");
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
  if (JSONContent.LED == "ON") {
    message = "LED ON";
    switchStatus = true;
  } else {
    message = "LED OFF";
    switchStatus = false;
  }
  console.log(message);
  document.getElementById("LED").innerHTML = message;
  document.getElementById("output").checked = switchStatus;
}

// Swtich click send data
function sendData(element) {
  var command;
  if (element.checked) command = '{"LED":"ON"}';
  else command = '{"LED":"OFF"}';
  websocket.send(command);
}
