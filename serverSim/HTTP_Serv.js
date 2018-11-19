var http = require('http');
var url = require('url');

/* http://xxx.xxx.xxx.xxx:port/?antrian=b */

var AntrianA  = 123;
var AntrianB  = 196;
var Loket = 'A';
var Tic = 0;

var server = http.createServer(function (req, res) {

    if (req.method === "GET") {
 		console.log("GET RECEIVED\n");

		query = url.parse(req.url, true).query;
		console.log(query);
		
		res.writeHead(200, { "Content-Type": "text/plan" });
		
		if (query.antrian == "a") {
			/* resp with current que of A */
			res.write(`${AntrianA}`);
			res.end();
		}
		
		if (query.antrian == "b") {
			/* resp with current que of B */
			res.write(`${AntrianB}`);
			res.end();
		}

		if (query.antrian == "x") {
			/* resp with current queue */
			if (Loket == 'A') {
				res.write(`${AntrianA}#${Loket}`);
			}
			if (Loket == 'B') {
				res.write(`${AntrianB}#${Loket}`);
			}
			res.end();
		}
		
	} else if (req.method === "POST") {
		console.log("POST RECEIVED\n");
		console.log(req.url);
		
		query = url.parse(req.url, true).query;
		console.log(query);

		res.writeHead(200, { "Content-Type": "text/plain" });

		if (query.antrian == "AN") {
			console.log("ANTRIAN A NEXT");
			Loket = 'A';
			AntrianA++;
			if (AntrianA > 999)
				AntrianA = 0;
		}

		if (query.antrian == "AR") {
			console.log("ANTRIAN A RECALL");
			Loket = 'A';
			AntrianA = AntrianA;
		}

		if (query.antrian == "BN") {
			console.log("ANTRIAN B NEXT");
			Loket = 'B';
			AntrianB++;
			if (AntrianB > 999)
				AntrianB = 0;
		}

		if (query.antrian == "BR") {
			console.log("ANTRIAN B RECALL");
			Loket = 'B';
			AntrianB = AntrianB;
		}
		res.end();
		
    }

}).listen(3000);