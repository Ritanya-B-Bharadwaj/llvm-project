// File: server.js
const express = require('express');
const multer = require('multer');
const path = require('path');
const { exec } = require('child_process');
const fs = require('fs');

const app = express();
const PORT = 3000;
const upload = multer({ dest: 'uploads/' });
const csvParse = require('csv-parse/sync');

app.use(express.urlencoded({ extended: true }));
app.use(express.static('public'));

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public/index.html'));
});

app.post('/run', upload.single('testfile'), (req, res) => {
  const inputPath = req.body.testfile;
  const outputFile = req.body.output || `function_metrics_${Date.now()}.csv`;
  const events = req.body.events;

  const command = `sudo bash ./run_pipeline.sh -i ./test/${inputPath} -e "${events}" -o ${outputFile}`;
  // const command = "sudo ./run_pipeline.sh -i './test/test_1.c' -e 'PAPI_L1_DCM,PAPI_TOT_INS' -o 'function_metrics.csv'";

  exec(command, (error, stdout, stderr) => {
    if (error) {
      return res.send(`<pre>Error running script:\n${stderr}</pre>`);
    }
    const csvPath = path.join(__dirname, outputFile);
    if (fs.existsSync(csvPath)) {
      const csvContent = fs.readFileSync(csvPath, 'utf8');
      const records = csvParse.parse(csvContent, { columns: true });

      const tableHtml = `
        <h2>Script Output:</h2><pre>${stdout}</pre>
        <h2>CSV Output:</h2>
        <table border="1" cellpadding="5" cellspacing="0">
          <thead>
            <tr>${Object.keys(records[0]).map(k => `<th>${k}</th>`).join('')}</tr>
          </thead>
          <tbody>
            ${records.map(row => `
              <tr>${Object.values(row).map(val => `<td>${val}</td>`).join('')}</tr>
            `).join('')}
          </tbody>
        </table>
      `;

      res.send(tableHtml);
    } else {
      res.send(`<pre>Script ran, but CSV not found.</pre>`);
    }
    
  });
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
