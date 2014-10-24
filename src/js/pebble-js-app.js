// Function to send a message to the Pebble using AppMessage API
var responses = ["Sir Lancelot of Camelot.", "To seek the Holy Grail.", "Blue."];

function sendMessage(message) {
  var req = new XMLHttpRequest();
  req.open('GET', "http://www.google.com", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log("http://www.google.com: " + req.status);
        Pebble.sendAppMessage({"status": 0, "message": message});
      } else {
        console.log("Error");
      }
    }
  }
  
  req.send(null);
}


// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
                console.log("JavaScript app ready and running!");
							});
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
                console.log("Received Status: " + e.payload.status + "; Request: " + e.payload.request);
                if (e.payload.request >= 0 && e.payload.request < responses.length) {
                  sendMessage(responses[e.payload.request]); 
                }
							});