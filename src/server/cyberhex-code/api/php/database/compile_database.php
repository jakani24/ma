<?php
// Function to sort words into files based on the first two characters
function sort_hashesh($inputFile) {
    // Open the input file for reading
    $handle = fopen($inputFile, "r");
    if ($handle === false) {
        die("Unable to open input file.");
    }
    
    // Read each line from the input file
    while (($line = fgets($handle)) !== false) {
        // Remove leading/trailing whitespace and split the line into words
       
		// Get the first two characters of the word
		$prefix = substr($word, 0, 2);
		
		// Create the filename for the corresponding file
		$filename = $prefix . ".txt";
		
		// Open or create the file for writing
		$fileHandle = fopen($filename, "a");
		if ($fileHandle === false) {
			die("Unable to open/create file: $filename");
		}
		
		// Write the word to the file
		fwrite($fileHandle, $word . PHP_EOL);
		
		// Close the file handle
		fclose($fileHandle);
			
		}
    
    // Close the input file handle
    fclose($handle);
}
function download_files(){
	
}
$url = 'http://example.com/file.txt';

// Initialize cURL session
$ch = curl_init($url);

// Set options
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

// Execute cURL session
$fileContents = curl_exec($ch);

// Check if the request was successful
if ($fileContents === false) {
    die('Failed to download file');
}

// Close cURL session
curl_close($ch);

// Save the downloaded file to disk
$localFilePath = 'downloaded_file.txt';
file_put_contents($localFilePath, $fileContents);

echo 'File downloaded successfully';
// Usage example: Provide the path to the input file
$inputFile = "input.txt";
//sortWordsIntoFiles($inputFile);

?>
