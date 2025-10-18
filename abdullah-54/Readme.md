## **To Execute Code**


#### First run this:
```bash
bison -d --language=c++ -o parser.tab.cpp parser.y -Wcounterexamples
```

#### then run 
``` bash
g++ -std=c++17 -o parser parser.tab.cpp lexer.cpp
```

#### and then for test run:
 ```bash
 Get-Content test.txt | ./parser
 ```
 ---