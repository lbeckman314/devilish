const { spawn } = require('child_process');
const child = spawn('./devilish');

child.stdin.setEncoding('utf-8');
child.stdout.pipe(process.stdout);

child.stdin.write("ls\n");
child.stdin.write("exit\n");
child.stdin.end();


child.stderr.on('data', (data) => {
  console.log(`stderr: ${data}`);
});

child.on('close', (code) => {
  console.log(`child process exited with code ${code}`);
});

