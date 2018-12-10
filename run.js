// setup express and handlebars
var express = require('express');

var app = express();
var handlebars = require('express-handlebars').create({defaultLayout:'main'});

app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');

// set port number to command line argument
app.set('port', process.argv[2]);

app.use(express.static('public'));


// home page (GET request)
app.get('/',function(req,res,next){
    var context = {};
    context.layout = false;

const { spawn } = require('child_process');

const child = spawn('wc');

process.stdin.pipe(child.stdin)

child.stdout.on('data', (data) => {
  console.log(`child stdout:\n${data}`);
});

    res.render('run', context);
});


app.listen(app.get('port'), function(){
  console.log('Express started on http://localhost:' + app.get('port') + '; press Ctrl-C to terminate.');
});

