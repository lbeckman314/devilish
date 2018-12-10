// https://developer.mozilla.org/en-US/docs/Web/API/WebSocket

var WebSocket = require('ws')
const readline = require('readline');

console.log("Connecting to server...");

process.env["NODE_TLS_REJECT_UNAUTHORIZED"] = 0;

// Create WebSocket connection.
const socket = new WebSocket('wss://192.168.0.15:8181');

console.log("Success!");


// Connection opened
socket.onopen = function (event) {
	//command = readline.question(": ");
	console.log("Sending initial message to server.");
	socket.send("");
}


// Listen for messages
socket.onmessage = function(event) {
	console.log(event.data);

	//let rl = require('readline-sync');
	//let comm = rl.question(': ');
	//socket.send(comm);


	const rl = readline.createInterface({
		input: process.stdin,
		output: process.stdout
	});

	//readline.moveCursor(process.stdout, 0, -1);
	rl.question(': ', (answer) => {
		// TODO: Log the answer in a database
		socket.send(answer);

		rl.close();
	});
}



