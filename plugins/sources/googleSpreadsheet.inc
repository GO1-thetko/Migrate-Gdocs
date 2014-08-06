<?php

use Google\Spreadsheet\Spreadsheet;
use Google\Spreadsheet\Worksheet;

/**
 * Implementation of MigrateSource, to handle imports from CSV files.
 *
 * If the CSV file contains non-ASCII characters, make sure it includes a
 * UTF BOM (Byte Order Marker) so they are interpreted correctly.
 */
class GoogleSpreadsheet extends MigrateSource
{
	/**
	 * @var Google\Spreadsheet\Spreadsheet
	 */
	protected $spreadsheet;

	/**
	 * @var GoogleSpreadsheetIterator
	 */
	protected $googleSpreadSheetIterator;

	/**
	 * @var \Google\Spreadsheet\Worksheet
	 */
	protected $worksheet, $worksheets;

	protected $fields, $worksheet_id;

	public function __construct($options, Spreadsheet $spreadsheet, $worksheet_id = 0, $fields = array())
	{
		parent::__construct($options);
		$this->spreadsheet = $spreadsheet;
		$this->fields = $fields;
		$this->worksheet_id = $worksheet_id;
	}

	/**
	 * @param $worksheet_id
	 * @return Worksheet
	 */
	private function getWorksheet($worksheet_id)
	{
		if (!$this->worksheet) {
			$this->worksheets = $this->spreadsheet->getWorksheets();
			$this->worksheet = $this->worksheets[$worksheet_id]; //Get the worksheet
		}

		return $this->worksheet;
	}

	/**
	 * Get the worksheet rows
	 * @param Worksheet $worksheet
	 * @return GoogleSpreadsheetIterator
	 */
	private function getWorksheetRows(Worksheet $worksheet)
	{
		if (!$this->googleSpreadSheetIterator) {
			$listFeed = $worksheet->getListFeed();
			$listEntries = $listFeed->getEntries();

			foreach($listEntries as $listEntry){
				$rows[] = $listEntry->getValues();
			}

			/**
			 * @var $listFeed \Google\Spreadsheet\ListFeed
			 */
			$this->googleSpreadSheetIterator = new GoogleSpreadsheetIterator($rows);
		}

		return $this->googleSpreadSheetIterator;
	}

	/**
	 * Get the header columns
	 */
	private function getHeaders(Worksheet $worksheet){
		//Get the listfeed from the worksheet
		$listFeed = $worksheet->getListFeed();
		return $listFeed->getColumnNames($listFeed->xml);
	}

	/**
	 * Returns a list of fields available to be mapped from the source query.
	 *
	 * @return array
	 *  Keys: machine names of the fields (to be passed to addFieldMapping)
	 *  Values: Human-friendly descriptions of the fields.
	 */
	public function fields()
	{
		$worksheet = $this->getWorksheet($this->worksheet_id);
		$header = $this->getHeaders($worksheet);

		$fields = array();
		foreach ($header as $value) {
			$fields[$value] = $value;
		}

		if ($this->fields) {
			$fields = $this->fields + $fields;
		}

		return $fields;
	}

	/**
	 * Return a count of all available source records.
	 */
	public function computeCount()
	{
		$worksheet = $this->getWorksheet($this->worksheet_id);
		$rows = $this->getWorksheetRows($worksheet);
		return $rows->computeCount();
	}

	/**
	 * Implementation of MigrateSource::performRewind().
	 */
	public function performRewind()
	{
		$worksheet = $this->getWorksheet($this->worksheet_id);
		$rows = $this->getWorksheetRows($worksheet);
		$rows->rewind();
	}

	/**
	 * Implementation of MigrationSource::getNextRow().
	 *
	 * @return stdClass
	 *  data for the next row from the JSON source files
	 */
	public function getNextRow()
	{
		$worksheet = $this->getWorksheet($this->worksheet_id);
		$rows = $this->getWorksheetRows($worksheet);

		$row = null;

		$rows->next();

		if ($rows->valid()) {
			$row = $rows->current();
			$row = $row->getValues();
			$row = (object)$row;
		} else {
			return null;
		}

		return $row;
	}
}

class GoogleSpreadsheetIterator implements Iterator
{
	protected $rows;
	private $position = 0;

	function __construct(array $rows)
	{
		$this->position = 0;
		$this->rows = $rows;
	}

	public function computeCount(){
		return sizeof($this->rows);
	}

	/**
	 * (PHP 5 &gt;= 5.0.0)<br/>
	 * Move forward to next element
	 * @link http://php.net/manual/en/iterator.next.php
	 * @return void Any returned value is ignored.
	 */
	public function next()
	{
		++$this->position;
	}

	/**
	 * (PHP 5 &gt;= 5.0.0)<br/>
	 * Return the key of the current element
	 * @link http://php.net/manual/en/iterator.key.php
	 * @return mixed scalar on success, or null on failure.
	 */
	public function key()
	{
		return $this->position;
	}

	/**
	 * (PHP 5 &gt;= 5.0.0)<br/>
	 * Checks if current position is valid
	 * @link http://php.net/manual/en/iterator.valid.php
	 * @return boolean The return value will be casted to boolean and then evaluated.
	 * Returns true on success or false on failure.
	 */
	public function valid()
	{
		return isset($this->rows[$this->position]);
	}

	/**
	 * (PHP 5 &gt;= 5.0.0)<br/>
	 * Rewind the Iterator to the first element
	 * @link http://php.net/manual/en/iterator.rewind.php
	 * @return void Any returned value is ignored.
	 */
	public function rewind()
	{
		$this->position = 0;
	}

	/**
	 * (PHP 5 &gt;= 5.0.0)<br/>
	 * Return the current element
	 * @link http://php.net/manual/en/iterator.current.php
	 * @return mixed Can return any type.
	 */
	public function current()
	{
		return $this->rows[$this->position];
	}
}