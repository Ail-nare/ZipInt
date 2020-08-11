# ZipInt

### Header based library.

Serialisation method that compressed the data for any numeric type and UTF-16/UTF-32 string (std::wstring).
The size of the number can be virtualy infinite.

---

#### How to use it

    ZipInt<bool _signed, uint16_t _compression_method, bool _dynamic_zip_int_type, bool _escape>

 - **\_signed**, (default: false): if true it'll means that the given data will be signed.
 - **\_compression\_method**, (default: 0): it describe the compression method that will be use, however their is currently only one.
 - **\_dynamic\_zip\_int\_type**, (default: false) if true than the all the given options as template will be writen with the serialisation to ensure that the writing and reading options are the same, but it will come with a lost of performance if abused.
 - **\_escape**, (default: false) if true the data will lose the ability to be virtualy infinite and will have a limit (depending on compression method), but a special character called 'escape' will be added. escape isn't a number it's a character to signal whatever the user want it to be.  

---

#### Example
###### - How To Write

    // You can use the static function Get() to get a singleton
    ZipInt<false>::Get().write(std::cout, 2); // This write 1 Byte to the stream witch contain the value 2
    
    // Or you can create an object of the class
    ZipInt<true> zipInt;
    zipInt.write(std::cout, -64); // this write 1 Byte to the stream witch contain the value -64
    zipInt.write(std::cout, -65); // this write 2 Byte to the stream witch contain the value -65

##### TODO
 - Reading, well it's kinda nice to be able to write data with **\_compression\_method**(0) but we still can't read it.
 - Escape, the **\_escape** option is still not implemented (or too little to work).
 - More test for **\_dynamic\_zip\_int\_type**, it's implemented but not tested, should be fine tho.
 - Adding to **\_compression\_method**(1, 2, 3, 4, 5, 6 and 7), witch are almost the same (those method are for little and Big number).
 - Adding to **\_compression\_method**(8) (this method is for Big Only).
 - Detail more the different **\_compression\_method**s.

/!\\ _Still in progress_
