/**
 * Je dáno celé číslo k.
 * Chceme najít přirozené číslo N ≥ 1 s nejmenším počtem cifer
 * (tj. co nejkratší číslo), které
 *   má v desítkovém zápise pouze číslice 0 a 1 a
 *   je dělitelné K,
 * případně oznámit, že takové číslo neexistuje.
 * Protože N ≥ 1, je zřejmé, že N musí začínat číslicí 1.
 * Nejkratší číslo v dekadické soustavě s binárními číslicemi dělitelné
 * číslem K = 3 je N = 111
 * číslem K = 4 je N = 100
 * číslem K = 5 je N = 10
 * číslem K = 6 je N = 1110
 * **/

#include <iostream>
#include <unordered_set>

int main() {
    int k = 0;
    std::cin >> k;
    if (k <= 0) {
        std::cout << "K musi byt kladne cele cislo > 0" << std::endl;
        return 1;
    }
    if(k == 1) {
        std::cout << k << std::endl;
        return 1;
    }
    int start = 1;
    std::vector<bool> visited(k, false);
    std::queue<std::pair<int, int>> q;
    q.push({start, start});
    while(!q.empty()) {
        auto [remainder, number] = q.front();
        q.pop();

        if(remainder == 0) {
            std::cout << number << std::endl;
            return 1;
        }

        for (int digit: {0, 1}) {
            int newNumber = (number * 10 + digit);
            int newRemainder = (number * 10 + digit) % k;
            if(!visited[newRemainder]) {
                q.push({newRemainder, newNumber});
                visited[newRemainder] = true;
            }
        }
    }
    std::cout << "No solutions" << std::endl;
    return 1;
}
