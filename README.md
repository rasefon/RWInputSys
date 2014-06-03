RWInputSys
==========
1. unsigned char rw_advance() 单步向前移动一个字符并返回，如果遇到EOF则返回0。
2. unsigned char rw_lookforward(int n)  向前看n个字符并返回，不实际移动当前扫描下标，如果超出范围，则返回0。
3. void rw_begin_curr_scan()  标记开始扫描当前词素，当前词素下标加一。
4. void rw_end_curr_scan()  标记结束当前扫描的词素。
5. unsigned char* rw_curr_token()  返回当前词素指针。
6. int rw_curr_token_len()  返回当前词素的长度。
7. int rw_curr_lineno() 返回当前词素的行号。
8. int rw_is_top()  判断是否处于文件开头，用于对正则式里'^'符号的综合判断。
9. int rw_openfile(const char *fn)  打开待扫描的文件。
为了避免实现的复杂性，我没有提供push back的功能，实际应用中，可以在词法分析时候用lookforward加自定义缓存代替。
