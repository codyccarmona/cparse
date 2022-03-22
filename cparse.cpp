#include <iostream>
#include <list>

struct ProductionRule{
    char symbol;
    std::list<char> production;
    
    ProductionRule(std::string rule){
        auto arrowEndingIndex = rule.find('>');
        auto productionItr = rule.begin();

        if(arrowEndingIndex != std::string::npos){
            symbol = *productionItr;
            productionItr += arrowEndingIndex + 1;               
        }
        else{
            symbol = '`';           
        }

        for( ; productionItr != rule.end(); productionItr++)
            production.push_back(*productionItr);
        
    }
};

int main(int argc, char *argv[]){
    std::list<ProductionRule> grammar;
    std::string readLine;

    /** Read grammar production rules for stdin */
    while(std::getline(std::cin, readLine)){
        ProductionRule rule(readLine);
        auto i = readLine.find('>');
        grammar.push_back(rule);
    }
    
    return 0;
}