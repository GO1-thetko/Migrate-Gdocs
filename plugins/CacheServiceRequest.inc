<?php

/**
 * Service Request. The parent class of all services.
 *
 * @package    Google
 * @subpackage Spreadsheet
 * @author     Asim Liaquat <asimlqt22@gmail.com>
 */
class CacheServiceRequest extends \Google\Spreadsheet\DefaultServiceRequest
{
	protected $cacheDirectory;

	public function setCacheDirectory($directory)
	{
		if (!is_writable($directory)) {
			throw new ErrorException('Cache Directory ' . $directory . ' is not writable');
		}

		$dirname = $directory.'/gdocs';

		//Create the directory
		if(!file_exists($dirname)){
			mkdir($dirname);
		}

		$this->cacheDirectory = $dirname;
	}

	/**
	 * Perform a get request
	 *
	 * @param string $url
	 *
	 * @return string
	 */
	public function get($url)
	{
		//Check cache directory first
		$file_name = md5($url).'.xml';
		if(file_exists($this->cacheDirectory.'/'.$file_name)){
			return file_get_contents($this->cacheDirectory.'/'.$file_name);
		}

		$ch = $this->initRequest($url);
		curl_setopt($ch, CURLOPT_CUSTOMREQUEST, 'GET');
		$xml = $this->execute($ch);

		if($this->cacheDirectory){
			file_put_contents($this->cacheDirectory.'/'.$file_name, $xml);
		}

		return $xml;
	}

	public function cacheClear()
	{
		$files = glob($this->cacheDirectory.'/*'); // get all file names
		foreach($files as $file){ // iterate files
			if(is_file($file))
				unlink($file); // delete file
		}
	}
}