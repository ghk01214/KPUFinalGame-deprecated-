**C++ Style Guide**
=====
**배경**  
------
<t>이 문서는 졸업작품 제작을 함에 있어서 코드 컨벤션을 사전에 지정함으로서 협업 중 혼선을 방지하는 것을 목표로 한다.

**이름 규칙**
------
1. 일반 이름 규칙  
   *  함수 이름, 변수 이름, 파일 이름은 약어를 피하고 서술적으로 지어야 한다.
   *  가능하면 상세한 이름을 사용하라. 새로 읽는 사람이 즉시 이해할 수 있는 것이 글자 길이를 줄이는 것보다 훨씬 중요하다.
   *  프로젝트에 관계되지 않은 사람이 익숙하지 않은 약어를 사용하지 말고 중간 글자를 지워서 축약하지 말라.
2. 파일 이름
   * 파일 이름은 대문자로 시작하며 언더스코어 없이 단어마다 첫 글자로 대문자를 사용한다.  
    ex) MyUsefulClass.h, MyExcitingFunction.cpp
   * db.h와 같이 /usr/include에 이미 존재하는 파일 이름은 사용하지 말라.
   * 일반적으로 상세하게 파일 이름을 지으라.
    ex) HttpServerLogs.h가 logs.h보다 좋다.
   * 인라인 함수는 .h에 있어야 한다. 인라인 함수의 코드가 짧으면 .h안에 들어가고, 길다면 Inl.h로 가야한다. 클래스 안에 많은 인라인 코드가 있다면 3개의 파일로 분리한다. 
    ``` C++
    UrlTable.h          // 클래스 선언
    UrlTable.cpp        // 클래스 정의
    UrlTableInl.h       // 많은 코드를 포함한 인라인 함수
    ```
   * Inl.h 파일 섹션 참조
3. 타입 이름  
   * 타입 이름은 대문자로 시작하며 언더스코어 없이 단어마다 첫 글자로 대문자를 사용한다.  
    ex) MyExcitingClass, MyExcitingEnum
   * 클래스, 구조체, typedef, 열거형(enum)을 포함한 모든 타입에 대해 같은 규칙이 적용된다.
    ``` C++
    // 클래스와 구조체
    class UrlTable {...}
    class UrlTableProperties {...}
    struct UrlTableProperties {...}

    // typedef
    typedef hash_map<UrlTableProperties*, std::string> PropertiesMap;
    
    // 열거형(enum)
    enum UrlTableErrors {...}
    ```
4. 변수 이름  
    * 변수의 이름은 모두 소문자로 작성하며 단어 사이에 언더스코어를 사용한다.
    * 클래스 멤버 변수는 이름의 앞에 m_ 접두어를 사용한다.  
     ex) my_exciting_local_variable, m_my_exciting_member_variable  
   
    <u>공통사항</u>
    ``` C++
    std::string table_name; // 좋음 - 언더스코어를 사용한다.
    std::string tablename;  // 좋음 - 모두 소문자이다.

    std::string tableName;  // 불가 - 대문자 사용
    ```
    <u>클래스 데이터 멤버</u>  
    데이터 멤버(인스턴스 변수 또는 멤버 변수)의 이름은 보통 변수처럼 소문자와 선택적인 언더스코어로 작성하지만, 항상 앞에 m_ 접두어를 붙인다.
    ``` C++
    std::string m_table_name;   // 좋음 - 앞에 m_가 있다.
    std::string m_tablename;    // 좋음
    ```
    <u>구조체 변수</u>  
    구조체 안에 있는 데이터 멤버는 클래스에 있는 데이터 멤버와 다르게 앞에 m_ 접두어를 붙이지 않고 보통 변수처럼 이름짓는다.
    ``` C++
    struct UrlTableProperties
    {
        std::string name;
        int num_entries;
    }
    ```
    어떤 경우에 클래스와 구조체를 써야 할 지에 대해서는 구조체 대 클래스를 참고하라.  
    <u>전역 변수</u>
    * 제한적으로 사용되어야 하는 전역 변수의 이름에 대한 특별한 규칙은 없다. 하지만 전역 변수를 사용할 때에는 g_와 같이 로컬 변수와 쉽게 구분할 수 있는 접두어를 사용하는 것이 좋다.
5. 상수 이름
    * 지역변수인지, 전역변수인지, 클래스의 일부인지와 상관없이 모든 컴파일 시점 상수들은 다른 변수들과 조금 다른 이름 규칙을 사용한다. k로 시작하여 매 단어의 첫 글자를 대문자로 쓴다.
    ``` C++
    const int kDaysInWeek = 7;
    ```
6. 함수 이름  
    * 일반 함수들은 대문자로 시작하며 언더스코어 없이 단어마다 첫 글자로 대문자를 사용한다. 접근자와 변경자는 해당하는 변수의 이름과 같은 것을 쓴다.  
     ex) MyExcitingFunction(), MyExcitingMethod()  
    
    <u>일반 함수</u>  
      * 함수 이름은 대문자로 시작하여 각 단어의 첫 글자를 대문자로 쓰고, 언더스코어는 사용하지 않는다.
      * 함수의 실행 중 크래시가 발생할 수 있다면 함수의 이름 뒤에 OrDie를 붙인다. 이 규칙은 프로덕션 코드에서도 에러가 발생할 가능성이 어느 정도 있는 함수에 한해 적용한다.
    ``` C++
    AddTableEntry()
    DeleteUrl()
    OpenFileOrDie()
    ```

    <u>접근자와 변경자</u>  
      * 접근자와 변경자 (get과 set 함수)는 접근 또는 변경을 하려는 변수의 이름과 일치하는 이름을 사용한다.
    ``` C++
    class MyClass
    {
    public:
    ...
        int NumEntries() const { return m_num_entries; }
        void SetNumEntries(int num_entries) { m_num_entries = num_entries; }

    private:
        int m_num_entries;
    };
    ```
7. 네임스페이스 이름  
    * 네임스페이스 이름은 모두 소문자로 하며, 프로젝트의 이름과 아마도 디렉토리 구조에 기반하여 작성한다.
     ex) kpu_final_game
    * 네임스페이스와 네임스페이스의 이름을 짓는 방식에 대해선 네임스페이스 섹션 참고.
8. 열거형 이름
    * 각각의 열거형은 상수처럼 이름짓는 것이 선호되지만 매크로처럼 이름짓는 것도 허용한다. 열거형의 이름은 타입이므로 대소문자를 섞어서 사용한다.
    ``` C++
    enum UrlTableErrors
    {
        kOK = 0,
        kErrorOutOfMemory,
        kErrorMalformedInput,
    };
    enum AlternateUrlTableErrors
    {
        OK = 0,
        OUT_OF_MEMORY = 1,
        MALFORMED_INPUT = 2,
    };
    ```
    * 매크로 방식은 매크로와 열거형 값의 충돌을 일으켰고, 상수 스타일의 이름을 선호하는 방식으로 규칙이 변경되었다.
    * 매크로 방식의 이름이 실제로 컴파일 오류를 발생시키는 것이 아니라면 코드를 상수 스타일 이름으로 수정할 필요는 없다.
9. 매크로 이름
    * 전처리기 매크로 섹션을 참고하라.
    * 일반적으로 매크로는 사용하지 않는 것이 좋다.
    * 하지만 절대적으로 필요하다면 대문자와 언더스코어로만 이름을 짓는다.
    ``` C++
    #define ROUND(X) ...
    #define PI_ROUNDED 3.0
    ```
10. 이름 규칙의 예외
     * 이미 존재하는 C 또는 C++ 엔티티와 비슷한 것의 이름을 짓는 경우 그 이름 규칙을 따를 수 있다.
    ``` C++
    bigopen()
        open()의 이름에 기반한 함수
    uint
        typedef
    bigpos
        pos와 같은 형식의 struct 또는 class
    sparse_hash_map
        STL과 유사한 엔티티이므로 STL의 이름 규칙을 따름
    LONGLONG_MAX
        INT_MAX와 비슷한 상수
    ```