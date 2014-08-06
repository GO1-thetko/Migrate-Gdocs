<?php
use Google\Spreadsheet\ServiceRequestInterface;

class TestServiceRequest implements ServiceRequestInterface
{
	private $retVal;

	public function setExecuteReturn($retVal)
	{
		$this->retVal = $retVal;
	}

	public function execute()
	{
		if ($this->retVal instanceof \Exception) {
			throw new $this->retVal;
		}

		return $this->retVal;
	}

	public function delete($url)
	{
		return $this->execute();
	}

	public function get($url)
	{
		if(strpos($url,'feeds/spreadsheets') !== FALSE){
			//Load the spreadsheet XML
			$xml = file_get_contents(__DIR__.'/xml/spreadsheet.xml');
			$this->setExecuteReturn($xml);
		}elseif(strpos($url,'feeds/worksheets') !== FALSE){
			//Load the worksheet-feed XML
			$xml = file_get_contents(__DIR__.'/xml/worksheet-feed.xml');
			$this->setExecuteReturn($xml);
		}elseif(strpos($url,'feeds/list') !== FALSE){
			//Load the list-feed XML
			$xml = file_get_contents(__DIR__.'/xml/list-feed.xml');
			$this->setExecuteReturn($xml);
		}

		return $this->execute();
	}

	public function post($url, $postData)
	{
		return $this->execute();
	}

	public function put($url, $postData)
	{
		return $this->execute();
	}

}