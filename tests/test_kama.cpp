#include <iostream>
#include <vector>
#include <cmath>
#include <stack>
#include <queue>
#include <climits>
#include <algorithm>
using namespace std;

int dx[4] = {0, 0, -1, 1};
int dy[4] = {1, -1, 0 , 0};


vector<vector<int>> threeSum(vector<int>& nums) {
    int n = nums.size();
    sort(nums.begin(), nums.end());
    vector<vector<int>> ans;
    int l = 0, r = n - 1;
    int mmin = INT_MAX, mmax = INT_MIN;
    while(l < r) {
        size_t t = ans.size();
        for(int i = l + 1; i < r; ++i) {
            mmin = min(mmin, nums[i]);
            mmax = max(mmax, nums[i]);
            if(nums[i] + nums[l] + nums[r] == 0)
                ans.push_back({nums[i], nums[l], nums[r]});
        }

        if(ans.size() > t) {
            --r;
            ++l;
        } else if(mmin + nums[r] + nums[l] > 0) {
            --r;
        } else if(mmax + nums[r] + nums[l] < 0)
            ++l;
    }

    return ans;
}

int main() {
    // char d = 'l';
    // char * t = NULL;
    // char * m = NULL;
    
    // __sync_bool_compare_and_swap(&m, t, &d);
    // cout << *m << ' ' << d << endl;
    vector<int> t = {-1,0,1,2,-1,-4};
    vector<vector<int>> ans = threeSum(t);
    
    return 0;
}
