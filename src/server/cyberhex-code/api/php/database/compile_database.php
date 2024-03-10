<?php
// Function to sort words into files based on the first two characters
function sort_hashes($inputFile, $excluded) {
    // Open the input file for reading
    $handle = fopen($inputFile, "r");
    if ($handle === false) {
        die("Unable to open input file.");
    }
    
    // Initialize an array to hold batched hashes
    $batchedHashes = array();
    
    // Read each line from the input file
    while (($line = fgets($handle)) !== false && !feof($handle)) {
        // Remove leading/trailing whitespace and split the line into words
        $hash = trim($line);
        
        // Check if the hash is in the excluded array
        if (in_array($hash, $excluded)) {
            continue; // Skip this hash
        }
        
        // Get the first two characters of the word
        $prefix = substr($hash, 0, 2);
        
        // Create the filename for the corresponding file
        $filename = "/var/www/html/database_srv/".$prefix . ".jdbf";
        
        // Add the hash to the batched array
        $batchedHashes[$filename][] = $line;
    }
    
    // Close the input file handle
    fclose($handle);
    
    // Write batched hashes to files
    foreach ($batchedHashes as $filename => $hashes) {
        // Open or create the file for writing
        $fileContents = implode('', $hashes);
        file_put_contents($filename, $fileContents, FILE_APPEND);
    }
}
function download_files($excluded){
    //download from virusshare
    $file_count=485;
    for($i=0;$i<$file_count;$i++){
        $fileNumber = sprintf('%05d', $i);
        $url="https://virusshare.com/hashfiles/VirusShare_$fileNumber.md5";
        $ch = curl_init($url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $fileContents = curl_exec($ch);
        file_put_contents("/var/www/html/database_srv/$fileNumber.md5", $fileContents);
        sort_hashes("/var/www/html/database_srv/$fileNumber.md5", $excluded);
    }
    //download from https://bazaar.abuse.ch/export/txt/md5/recent/
    $url="https://bazaar.abuse.ch/export/txt/md5/recent/";
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $fileContents = curl_exec($ch);
    file_put_contents("/var/www/html/database_srv/buf.md5", $fileContents);
    sort_hashes("/var/www/html/database_srv/buf.md5", $excluded);
}



include "../../../config.php";
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
if ($conn->connect_error) {
	$success=0;
	die("Connection failed: " . $conn->connect_error);
}
// Load excluded hashes from sig_ex table
$excluded = array();
$sql = "SELECT signature FROM sig_ex";
$result = $conn->query($sql);
if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $excluded[] = $row["signature"];
    }
}

// Load included hashes from sig_in table
$included = array();
$sql = "SELECT signature FROM sig_in";
$result = $conn->query($sql);
if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $included[] = $row["signature"];
    }
}

// Close the database connection
$conn->close();

// Add included array to output files this will then be processed like any other normal input file
file_put_contents("/var/www/html/database_srv/included_hashes.txt", implode(PHP_EOL, $included));

// This code updates and compiles our databases
$directory = '/var/www/html/database_srv'; // Path to the directory

// Get a list of all files in the directory
$files = glob($directory . '/*');

// Iterate over each file and delete it
foreach ($files as $file) {
    // Check if the file is a regular file (not a directory)
    if (is_file($file)) {
        unlink($file);
    }
}
set_time_limit(0);
download_files($excluded);
?>
