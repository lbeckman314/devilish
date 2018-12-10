const fs = require('fs');
const https = require('https');
const WebSocket = require('ws');

const server = new https.createServer({
  cert: fs.readFileSync('/path/to/cert.pem'),
  key: fs.readFileSync('/path/to/key.pem')
});

const wss = new WebSocket.Server({ server });

wss.on('connection', function connection(ws) {

	const { spawn } = require('child_process');
	const options = {
		detached: true,
		stdio: 'pipe'
	};

	const child = spawn('bash', ['-c', './devilish'], options);
	child.unref();

	//const child = spawn('./devilish');

	//console.log("child:", child);

	child.stdin.setEncoding('utf-8');
	//child.stdout.pipe(process.stdout);


		//let res = ""
		child.stdout.on('data', (data) => {
			//console.log(`stdout: ${data}`);
			//console.log("SENDING TO CLIENT:", data.toString());
			//res += data.toString();
			ws.send(data.toString());
			//return res;
			//res = ""
		});


		child.stderr.on('data', (data) => {
			console.log(`stderr: ${data}`);
		});

		child.on('close', (code) => {
			console.log(`child process exited with code ${code}`);
		});


	ws.on('message', function incoming(message) {
		console.log('received: %s', message);

		child.stdin.write(message + "\n");
		//child.stdin.write("exit\n");
		//child.stdin.end();

	});

});

server.listen(8080);
