#include <iostream>
#include <vector>
#include <cmath>
#include <stack>
#include <queue>
#include <climits>
using namespace std;

int dx[4] = {0, 0, -1, 1};
int dy[4] = {1, -1, 0 , 0};


int main() {
    int n, m, j, k;
    cin >> n >> m >> j >> k;
    vector<vector<int>> grid(m, vector<int>(n, 0));
    for(int i = 0; i < m; i++) {
        for(int t = 0; t < n; t++) {
            cin >> grid[i][t];
        }
    }
    
    vector<vector<bool>> trix(m, vector<bool>(n, false));
    vector<vector<int>> dp(m, vector<int>(n, INT_MAX));
    dp[j][k] = 0;
    queue<pair<int, int>> st;
    st.push(make_pair(j, k));
    while(st.size() != 0) {
        pair<int, int> idx = st.front();
        st.pop();
        
        for(int i = 0; i < 4; i++) {
            int x = idx.first + dx[i];
            int y = idx.second + dy[i];
            if((x >= 0 && x < m) && (y >= 0 && y < n)) {
                if(grid[x][y] != 0 && !trix[x][y]) {
                    dp[x][y] = min(dp[x][y], grid[idx.first][idx.second] + dp[idx.first][idx.second]);
                    st.push(make_pair(x, y));
                }
                trix[x][y] = true;
            }
        }
    }
    int ans = 0;
    for(int i = 0; i < m; i++) {
        for(int t = 0; t < n; t++) {
            if(grid[i][t] != 0 && dp[i][t] == INT_MAX) {
                cout << -1 << endl;
                return 0;
            }
            
            if(grid[i][t] != 0) {
                ans = max(ans, dp[i][t]);
            }
        }
    }
    
        cout << ans << endl;
    
    return 0;
}
