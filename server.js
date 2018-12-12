const fs = require('fs');
const https = require('https');
const WebSocket = require('ws');

const server = new https.createServer({
  cert: fs.readFileSync('/etc/letsencrypt/live/liambeckman.com/cert.pem'),
  key: fs.readFileSync('/etc/letsencrypt/live/liambeckman.com/privkey.pem'),
  passphrase: '284846'
});

const wss = new WebSocket.Server({ server });

console.log("Waiting for clients...");

wss.on('connection', function connection(ws) {
    console.log("Client connected!");

	const { spawn } = require('child_process');
	const options = {
		detached: true,
		stdio: 'pipe'
	};

	const child = spawn('bash', ['-c', './devilish'], options);

    /*
    switch(program) {
        case "devilish":
            const child = spawn('bash', ['-c', './devilish'], options);
            break;
        case "prime":
            const child = spawn('bash', ['-c', './prime'], options);
            break;
        case "palindrome":
            const child = spawn('bash', ['-c', './palindrome'], options);
            break;
        default:
            console.log("Invalid program.");
    }
    */

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
		//console.log('received: %s', message);

		child.stdin.write(message + "\n");
		//child.stdin.write("exit\n");
		//child.stdin.end();

	});

});

server.listen(8181);
