#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

bool isTerminal(char c){
    return (!isupper(c) && c != '@' && c != '\'');
}

struct ProductionRule{
    ProductionRule(std::string rule, char parserSymbol = '\0') {
        auto productionItr = rule.begin();
        auto productionStartIndex = rule.find('>');

        if(productionStartIndex != std::string::npos){
            this->symbol = *productionItr;
            productionItr += productionStartIndex + 1;               
        }
        else{
            this->symbol = '`';           
        }

        if(parserSymbol != '\0')
            this->production.push_back(parserSymbol);

        for( ; productionItr != rule.end(); productionItr++)
            this->production.push_back(*productionItr);
    }

    /* Copy constructor that returns   */
    ProductionRule(const ProductionRule& p, char parserIndexSymbol = '\0'){
        this->symbol = p.symbol;

        if(parserIndexSymbol != '\0')
            this->production.push_back(parserIndexSymbol);

        for(auto itr = p.production.begin(); itr != p.production.end(); itr++)
            this->production.push_back(*itr);
    }

    const std::list<char>::iterator next(const std::list<char>::iterator itr){
        auto next = itr;

        if(next != this->production.end())
            next++;

        return next;
    }

    const std::list<char>::iterator find(char c){
        auto itr = this->production.begin();

        for(; itr != this->production.end(); itr++){
            if(*itr == c)
                break;
        }

        return itr;
    }

    friend bool operator==(const ProductionRule& l,  const ProductionRule& r){
        if(l.symbol != r.symbol || r.production.size() != l.production.size())
            return false;

        auto litr = l.production.begin();
        auto ritr = r.production.begin();

        while(litr != l.production.end() && ritr != r.production.end()){
            if(*litr != *ritr)
                return false;
            litr++;
            ritr++;
        }

        return true;
    }

    bool matchSymbol(char c){
        return symbol == c;
    }
private:
    char symbol;
    std::string production;
};

struct Grammar{
    Grammar(std::list<std::string> rules){
        for(auto strRulesItr = rules.begin(); strRulesItr != rules.end(); strRulesItr++)               
            this->rules.push_back(ProductionRule(*strRulesItr));
    }

    std::list<ProductionRule> matchProductionsForSymbol(char c){
        auto p = std::list<ProductionRule>();
        for(auto itr = this->begin(); itr != this->end(); itr++){
            if(itr->matchSymbol(c))
                p.push_back(ProductionRule(*itr));
        }
        return p;
    }

    const std::list<ProductionRule>::iterator begin(){
        return this->rules.begin();
    }

    const std::list<ProductionRule>::iterator end(){
        return this->rules.end();
    }

    const std::list<ProductionRule>::iterator next(const std::list<ProductionRule>::iterator itr){
        auto next = itr;

        if(next != this->end())
            next++;

        return next;
    }
private:
    std::list<ProductionRule> rules;
};

struct Item{
    Item(Grammar g){
        this->rules.push_back(ProductionRule(*g.begin(), '@'));
        for(auto itr = rules.begin(); itr != rules.end(); itr++){
            closure(*itr, g);
        }
    }
private:
    void closure(ProductionRule p, Grammar g){
        char symbolAfterParse = *(p.next(p.find('@')));
        std::list<ProductionRule> newRules = g.matchProductionsForSymbol(symbolAfterParse);

        for(auto i = newRules.begin(); i != newRules.end(); i++)
            this->addRule(ProductionRule(*i, '@'));       
    }

    void addRule(ProductionRule p){       
        for(auto itr = rules.begin(); itr != rules.end(); itr++){
            if(*itr == p)
                return;
        }
        rules.push_back(p);
    }

    std::list<ProductionRule> rules;
    std::list<std::pair<char, int>> gotoSymbols;
};
struct Set{
    Set(Grammar g){
        Item i = Item(g);
    }
private:
    std::list<Item> items;
};
std::list<std::string> readGrammar(){
    std::list<std::string> lines;
    std::string readLine;

    /** Read grammar production rules for stdin */
    while(std::getline(std::cin, readLine))
        lines.push_back(readLine);

    return lines;
}


int main(int argc, char *argv[]){
    Grammar grammar = Grammar(readGrammar());
    Item item = Item(grammar);
    return 0;
}