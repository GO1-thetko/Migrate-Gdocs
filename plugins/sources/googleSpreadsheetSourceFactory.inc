<?php

use Google\Spreadsheet\SpreadsheetService;
use Google\Spreadsheet\DefaultServiceRequest;
use Google\Spreadsheet\ServiceRequestFactory;

/**
 * This class initialises GoogleSpreadsheet
 * Class googleSpreadsheetSourceManager
 */
abstract class googleSpreadsheetSourceFactory
{

	static protected $access_token;

	public static function getGoogleSpreadsheetSource(SpreadsheetService $spreadsheetService, $id)
	{
		try{
			ServiceRequestFactory::getInstance();
		}catch(\Google\Spreadsheet\Exception $e){
			setMigrateGDocsServiceRequest(); //Set the migrateDocsServiceRequest
		}

		return new GoogleSpreadsheet(array(), $spreadsheetService->getSpreadsheetById($id));
	}

	public static function setServiceRequest($auth, Google_Client $google_Client, $cacheDirectory = "/tmp")
	{

		if ($google_Client->getAuth()->isAccessTokenExpired()) {
			$scopes = "https://spreadsheets.google.com/feeds/";

			$cred = new Google_Auth_AssertionCredentials(
				$auth['client_email'],
				array($scopes),
				$auth['private_key'],
				$auth['private_key_password']
			);

			$google_Client->setAssertionCredentials($cred);
			$google_Client->getAuth()->refreshTokenWithAssertion($cred);
		}

		$access_token = json_decode($google_Client->getAccessToken());
		$serviceRequest = new CacheServiceRequest($access_token->access_token);
		if(!empty($cacheDirectory)){
			$serviceRequest->setCacheDirectory($cacheDirectory);
		}


		static::$access_token = $access_token;
		ServiceRequestFactory::setInstance($serviceRequest);
	}

	public static function getAccessToken()
	{
		return static::$access_token;
	}
}