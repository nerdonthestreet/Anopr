<?php

// Applications can call this file with a Composr username to find out that user's email address.
// They should expect a string with a single email address in return.

// One input is required when calling this file.
$input_username = $argv[1];

// Configure a MySQL/MariaDB account with read access to Composr's cms_f_members table here.
$servername = "";
$username = "";
$password = "";
$dbname = "";

// Connect via SSL/TLS.
$pdo_options = [
 PDO::MYSQL_ATTR_SSL_CA => '',
 PDO::MYSQL_ATTR_SSL_CERT => '',
 PDO::MYSQL_ATTR_SSL_KEY => '',
];

// Get the email address of the Composr user with the provided username.
try {
  $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password, $pdo_options);
  $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
  $stmt = $conn->prepare("SELECT m_email_address FROM cms_f_members WHERE m_username = :input_username");
  $stmt->bindParam(":input_username", $input_username, PDO::PARAM_STR);
  if($stmt->execute()) {
    if($stmt->rowCount() == 1){
      if($row = $stmt->fetch()){
        $email_address = $row["m_email_address"];
      }
    }
  }
} catch(PDOException $e) {
  echo "Error: " . $e->getMessage();
}
$conn = null;

// Return the email address, or a placeholder if one was not found.
if (!empty($email_address)) {
  echo $email_address;
} else {
  echo "invalidircemail@example.com";
}

?>
