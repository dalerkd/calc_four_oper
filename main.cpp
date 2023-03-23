#include <iostream>
#include <string>
#include <cassert>


class Four_Oper_Analy
{
private:
    class DECIDE_END
    {

    };

    enum type
    {
        E_UNVALID = 0,
        E_NUMBER = 1,
        E_OPER = 2,
        E_END = 3
    };

    struct stru
    {
        stru() {
            type = E_UNVALID;
            number = 0;
            oper = 'Y';
        }
        type type;
        int number;
        char oper;
    };

public:
    void setBuffer(std::string buffer)
    {
        m_buffer = buffer;
        m_exp_now = const_cast<char*>(m_buffer.c_str());
        next();
    }
private:
    char* m_exp_now = nullptr;
    std::string m_buffer;
    stru m_stru_;
    stru get_token()
    {
        return m_stru_;
    }
    void set_token(stru t)
    {
        m_stru_ = t;

        printf("set type: ");
        char* type;
        switch (t.type)
        {
        case E_UNVALID:
            printf("UNVALID\r\n");
            break;
        case E_NUMBER:
            printf("NUMBER, Value:%d\r\n", t.number);
            break;
        case E_OPER:
            printf("OPER, Value:%c\r\n", t.oper);
            break;
        case E_END:
            printf("END, END,遇到终止\r\n");
            break;
        }
    }
    /*
     * 获取下一个token
     *
     * Exception:
     * 1. DECIDE_END 终止符
     * 2. char* : 语法错误等
     *
     */

    void next()
    {
        if (m_buffer.empty())
        {
            throw("m_buffer is empty\r\n");
        }

        stru stru_save;
        // add safe limit 要判断当前的操作是不是无效
        while (true) {
            {
                int all_size = m_buffer.size();

                int c = m_exp_now - m_buffer.c_str() + 1;
                if (c > all_size)
                {
                    if (stru_save.type == E_NUMBER)
                    {
                        return;
                    }
                    else
                    {
                        stru_save.type = E_END;
                        set_token(stru_save);
                        printf("遇到终止\r\n");
                        throw(DECIDE_END());
                    }
                }
            }
            char tmp = *m_exp_now;
            switch (tmp) {
            case '0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':
            {
                if (stru_save.type != E_NUMBER && stru_save.type != E_UNVALID)
                {
                    m_exp_now--;
                    set_token(stru_save);
                    return;
                }

                stru_save.type = E_NUMBER;
                stru_save.number = stru_save.number * 10 + (tmp - '0');
                set_token(stru_save);
                break;
            }
            case '+':case'-':case'*':case'/':case'(':case')':
            {
                if (stru_save.type != E_UNVALID)
                {
                    return;
                }
                stru_save.type = E_OPER;
                stru_save.oper = tmp;
                set_token(stru_save);
                m_exp_now++;
                return;
                break;
            }
            case ' ':case'\t':
            {
                break;
            }
            default:
            {
                printf("未预期的字符:%c(%d)\r\n", tmp, tmp);
                throw("无效的字符");
            }
            }
            m_exp_now++;
        }
    }

    void IsEnd()
    {
        if (get_token().type == E_END)
        {
            throw(DECIDE_END());
        }
    }
    void mustIsOper()
    {
        if (get_token().type != E_OPER)
        {
            throw("期待的标识符为OPER");
        }
    }
    void mustIsNumber()
    {
        if (get_token().type != E_NUMBER)
        {
            throw("期待的标识符为NUMBER");
        }
    }
    //不是操作符会异常
    char get_oper()
    {
        mustIsOper();
        return get_token().oper;
    }
    //不是数字会异常
    int get_number()
    {
        mustIsNumber();
        return get_token().number;
    }

    /*
     *

    Term = <数字> | "(" Exp ")"
    Factor = Term (("*"|"/")Term)*
    Exp = Factor (("+"|"-")Factor)*

        */
    int parseExp()
    {
        int tmp = parseFactor();
        /*
         * 怎么表达这种可以不存在的东西?
         * 1. 抛一个对象出来,当没有任何东西时
         * 2. match_end 匹配空
         *
         */
        while (true)
        {
            char op = 'Y';
            try {
                IsEnd();
                op = get_oper();
            }
            catch (DECIDE_END&)
            {
                return tmp;
            }
            if (op != '+'&&op != '-')
            {
                break;
            }
            next();
            // 继续获取下一个 Factor
            int secondResult = parseFactor();

            if (op == '+')
            {
                tmp += secondResult;
            }
            else if (op == '-')
            {
                tmp -= secondResult;
            }
            else
            {
                throw("期待得到+-,结果得到:%c(%d)", op, op);
            }
        }///-end-while
        return tmp;

    }

    int parseTerm()
    {
        int number;
        try {
            number = get_number();
            next();
            return number;
        }
        catch (DECIDE_END&)
        {
            return number;
        }
        catch (...)
        {
            ;
        }

        char L = get_oper();
        if (L != '(')
        {
            throw("期待'(',结果是:%c(%d)", L, L);
        }
        next();

        int result = parseExp();

        char R = get_oper();
        if (R != ')')
        {
            throw("期待')',结果是:%c(%d)", R, R);
        }
        try {
            next();
        }
        catch (DECIDE_END&)
        {
        }
        return result;


    }
    int parseFactor()
    {
        int tmp = parseTerm();
        while (true)
        {
            char op = 'Y';
            try {
                IsEnd();
                op = get_oper();
            }
            catch (DECIDE_END&)
            {
                return tmp;
            }


            if (op != '*'&&op != '/')
            {
                break;
            }
            next();
            // 继续获取下一个 Term
            int secondResult = parseTerm();

            if (op == '*')
            {
                tmp *= secondResult;
            }
            else if (op == '/')
            {
                if (secondResult == 0)
                {
                    throw("尝试除0");
                }
                tmp /= secondResult;
            }
            else
            {
                throw("期待得到+-,结果得到:%c(%d)", op, op);
            }



        }///-end-while
        return tmp;
    }

public:
    /*入口*/
    int calc()
    {
        try {
            return parseExp();
        }
        catch (DECIDE_END&)
        {
            printf("遭遇意料外的终止\r\n");
        }
        catch (char* p)
        {
            printf("Exception:%s\r\n", p);
        }
    }
    static void testSelf()
    {
        {
            Four_Oper_Analy foa;
            foa.setBuffer("1+2*3");
            assert(7 == foa.calc());
        }
        {
            Four_Oper_Analy foa;
            foa.setBuffer("10+2*3-(20+20)");
            assert(-24 == foa.calc());
        }
        printf("测试通过 :)\r\n");
    }
};

int main()
{
    Four_Oper_Analy test;
    test.testSelf();

    {
        Four_Oper_Analy foa;
        std::string buffer;
        while ((std::getline(std::cin, buffer))) {
            //
            foa.setBuffer(buffer);
            int m_number;

            m_number = foa.calc();

            printf("%d\n", m_number);
        }
    }

}
// 大约创建于 2019/3/26 或之前
