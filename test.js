const express = require('express')
const app = express()
const port = 3000
const fs = require("fs")
var path = require('path');

var log

app.use(express.urlencoded({ extended: true }))
app.get('/', function (req, res) {
	res.sendFile(path.join(__dirname + '/index.html'));
});
app.route('/data')
	.post((req, res) => {
		let fan = req.body.fan,
			temp = req.body.temp,
			time = new Date(),
			Time = time.toLocaleString()
		log = req.body

		res.send(log)

		let data = "Fan1 = " + fan + " || " + "Temp1 = " + temp + " || " + Time

		//fs.appendFile('log.txt', data + '\n', 'utf8', (err) => { if (err) throw err })
	})

	.get((req, res) => res.send(log))

app.listen(port, () => console.log(`Example app listening on port ${port}!`))