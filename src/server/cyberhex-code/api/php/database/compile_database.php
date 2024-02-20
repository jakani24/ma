<?php
// Function to sort words into files based on the first two characters
function sort_hashes($inputFile) {
    // Open the input file for reading
    $handle = fopen($inputFile, "r");
    if ($handle === false) {
        die("Unable to open input file.");
    }
    
    // Read each line from the input file
    while (($line = fgets($handle)) !== false) {
        // Remove leading/trailing whitespace and split the line into words
       
		// Get the first two characters of the word
		$prefix = substr($line, 0, 2);
		
		// Create the filename for the corresponding file
		$filename = "/var/www/html/database_srv/".$prefix . ".jdbf";
		
		// Open or create the file for writing
		$fileHandle = fopen($filename, "a");
		if ($fileHandle === false) {
			die("Unable to open/create file: $filename");
		}
		
		// Write the word to the file
		fwrite($fileHandle, $line);
		
		// Close the file handle
		fclose($fileHandle);
			
		}
    
    // Close the input file handle
    fclose($handle);
}
function download_files(){
	//download from virusshare
	$file_count=485;
	for($i=0;$i<$file_count;$i++){
		$fileNumber = sprintf('%05d', $i);
		$url="https://virusshare.com/hashfiles/VirusShare_$fileNumber.md5";
		$ch = curl_init($url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		$fileContents = curl_exec($ch);
		file_put_contents("/var/www/html/database_srv/$fileNumber.md5", $fileContents);
		sort_hashes("/var/www/html/database_srv/$fileNumber.md5");
	}
	//download from https://bazaar.abuse.ch/export/txt/md5/recent/
	$url="https://bazaar.abuse.ch/export/txt/md5/recent/";
	$ch = curl_init($url);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	$fileContents = curl_exec($ch);
	file_put_contents("/var/www/html/database_srv/buf.md5", $fileContents);
	sort_hashes("/var/www/html/database_srv/buf.md5");
}
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
download_files();


?>
