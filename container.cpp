#include <string>
#include <vector>
#include <algorithm>
/*
 * Структура в которой содержится множитель и степень, вектора таких структур будет достаточно для хранения
 * информации о входных и выходных многочленах
 * можно также использовать списки
 */
struct Members
{
    Members(){factor = 0; power = 0;}
    int factor;
    int power;
//перегрузка операторов сравнения, необходимых для использования в векторах
    bool operator< (const Members rh)
    {
        return ((this-> power) < (rh.power));
    }
    bool operator== (const Members rh)
    {
        return ((this-> power) == (rh.power));
    }

};
class Derivative
{
public:
    Derivative();
    std::string derivative(std::string polynomial);
    //скрываем всё не нужное
private:
    std::vector<Members> parseString(std::string polynomial);
    std::vector<Members> simplification(std::vector<Members>);
    std::vector<Members> derivativation(std::vector<Members>);
    std::string generationOutputString(std::vector<Members>);
};



Derivative::Derivative()
{

}
//искомая функция, возвращающая производную многочлена
std::string Derivative::derivative(std::string polynomial)
{
    return generationOutputString(derivativation(simplification(parseString(polynomial))));
}

//функция которая парсит входную строку, и представляет многочлен в виде вектора множителей и степеней
std::vector<Members> Derivative::parseString(std::string polynomial)
{
    std::vector<Members> outVector;
    //проходимся по строке, по сути тут мы переходим между челенами многочлена
    //в каждом цикле следим за границами
    for (uint32_t i = 0; i < polynomial.size();)
    {
        //пропускаем все пробелы
        if (polynomial[i] == ' ')
        {
            i++;
            continue;
        }
        std::string factorBuf;
        std::string powerBuf;
        Members buf;
        bool haveFactor = false;
        bool haveX = false;
        bool havePower = false;
        // в начале каждого члена находится либо множитель, либо "-", либо "х"
        while (((polynomial[i] != 'x') && (polynomial[i] != '+') /*|| (polynomial[i] != '-')*/ && (polynomial[i] != 'X')&& (polynomial[i] != '*')) && (i < polynomial.size()))
        {
            // опять же скипаем пробелы
            if (polynomial[i] == ' ')
            {
                i++;
                continue;
            }
            // если мы уже считали множитель, то значит мы дошли до следующего минуса (первый минус мы заносим в множитель)
            if (haveFactor && (polynomial[i] == '-'))
                break;
            haveFactor = true;
            factorBuf += polynomial[i];
            i++;
        }
        // если после множителя у нас умножение, то нужно найти х
        if (polynomial[i] == '*')
        {
            while (((polynomial[i] != 'x') && (polynomial[i] != 'X')) && (i < polynomial.size()))
            {
                if (polynomial[i] == ' ')
                {
                    i++;
                    continue;
                }
                i++;
            }
        }
        // после х, нам нужно найти либо знак возведения в степень, либо + либо минус
        if ((polynomial[i] == 'x')  || (polynomial[i] == 'X'))
        {
            haveX = true;
            while (((polynomial[i] != '+') && (polynomial[i] != '^') && (polynomial[i] != '-')) && (i < polynomial.size()))
            {
                if (polynomial[i] == ' ')
                {
                    i++;
                    continue;
                }
                i++;
            }
        }
        // если у нас есть знак возведения в степень, то  считываем степень в буфер
        if (polynomial[i] == '^')
        {
            i++;
            havePower = true;
            while ((polynomial[i] != '+') && (polynomial[i] != '-') && (i < polynomial.size()) )
            {
                if (polynomial[i] == ' ')
                {
                    i++;
                    continue;
                }
                powerBuf += polynomial[i];
                i++;
            }
        }
        // дошли до следующего члена(то есть до + или минуса, или вообще до конца многочлена)
        if ((polynomial[i] == '+') || (polynomial[i] == '-') || i == polynomial.size())
        {
            // если у нас был х
            if (haveX)
            {
                // и была степень...
                if (havePower)
                {
                    buf.power = std::stoi(powerBuf);
                }
                else //...иначе степень 1
                {
                    buf.power = 1;
                }
                //если был х и не было множителя, то множитель 1
                if ((!haveFactor) && (factorBuf.size() < 1))
                {
                    buf.factor = 1;
                }
            }
            // если был множитель, то
            if ((haveFactor) && (factorBuf.size() > 0))
            {
                // обход считывания одного минуса
                if (!(factorBuf.compare("-")))
                    buf.factor = -1;
                else
                    buf.factor = std::stoi(factorBuf);
            }
            // записываем член в вектор
            outVector.push_back(buf);
            // плюсы последующих членов мы не учитываем, хотя могли бы
            if (polynomial[i] == '+')
                i++;
        }


    }
    return outVector;
}

// упрощение многочлена, то есть приведение и сортировка
std::vector<Members> Derivative::simplification(std::vector<Members> innerBuf)
{
    std::vector<Members> outBuf;
    //1й элемент точно попадает в список
    outBuf.push_back(innerBuf[0]);
    for (uint32_t i = 1; i < innerBuf.size(); i++)
    {
        bool isInOut = false;
        // если находим элемент в выходном списке, то плюсуем множители, если нет, то он попадает в список
        for (uint32_t j = 0; j < outBuf.size(); j++)
            if (innerBuf[i] == outBuf[j])
            {
                isInOut = true;
                outBuf[j].factor += innerBuf[i].factor;
                break;
            }
        if (!isInOut)
            outBuf.push_back(innerBuf[i]);

    }
    //сортировка по позврастанию
    std::sort(outBuf.begin(),outBuf.end());

    return outBuf;
}

//обрабаытваем вектор в производную многочлена
std::vector<Members> Derivative::derivativation(std::vector<Members> innerBuf)
{
    // тк сортировка по возврастанию, то идём с конца
    std::vector<Members> outBuf;
    std::vector<Members>::iterator einp = innerBuf.end();
    std::vector<Members>::iterator binp = innerBuf.begin();

    do
    {
        einp--;
        // если степень 0 то сразу отбрасываем
        if (einp->power)
        {
            einp->factor *= einp->power;
            (einp->power)--;
            outBuf.push_back(*einp);
        }

    }while (einp != binp);
    return outBuf;

}

// генерируем выходную строку из готового вектора производной
std::string Derivative::generationOutputString(std::vector<Members> innerBuf)
{
    std::string outbuf;
    for (uint32_t i = 0; i < innerBuf.size(); i++)
    {
        // если множитель 0, то отбрасываем
        if (innerBuf[i].factor != 0)
        {
            // если множитель 1 или -1
            if ((innerBuf[i].factor == 1) || (innerBuf[i].factor == -1))
            {
                if (innerBuf[i].power)
                {
                    // конкретно если минус
                    if (innerBuf[i].factor == -1)
                    {
                        outbuf += '-';
                    }
                    //если степень есть добавляем х
                    outbuf += 'x';
                    // если она больше 1 то добавляем её
                    if (innerBuf[i].power > 1)
                    {
                        outbuf += '^';
                        outbuf += std::to_string(innerBuf[i].power);
                    }
                }
                else
                {
                    // если степени нет, то только множитель
                    outbuf += std::to_string(innerBuf[i].factor);
                }
            }
            else
            {
                //если факториал больше 1 то добавляем его
                outbuf += std::to_string(innerBuf[i].factor);
                // если хоть какая то степень есть, то *х
                if (innerBuf[i].power)
                {
                    outbuf += "*x";
                    //если она больше 1 то и её
                    if (innerBuf[i].power > 1)
                    {
                        outbuf += '^';
                        outbuf += std::to_string(innerBuf[i].power);
                    }
                }
            }
            //если ещё не всё перебрали, и следующий член положительный, то добавляем +
            if (i != (innerBuf.size() - 1) )
            {
                if (innerBuf[i + 1].factor > 0)
                    outbuf += '+';
//                else if (innerBuf[i + 1].factor < 0)
//                    outbuf += '-';
            }

        }
    }
    return outbuf;
}

std::string derivative(std::string polynomial) {
    std::string ret = "";
    Derivative obj;
    ret = obj.derivative (polynomial);
    return ret;
}




