#include <bits/stdc++.h>
using namespace std;

class Bucket {

    public:
        int depth, size;
        vector<int> values;

        Bucket(int depth, int size)
        {
            this->depth = depth;
            this->size = size;
        }

        int insert(int key)
        {
            bool keyExists= false;
            for(int i=0; i<values.size(); i++)
            {
                if(values[i]== key)
                {
                    keyExists= true;
                    break;
                }
            }

            if(keyExists)
                return -1;
            if(values.size() == size)
                return 0;
            values.push_back(key);
            return 1;
        }

        int remove(int key)
        {
            vector<int>::iterator it = find(values.begin(), values.end(), key);
            if(it != values.end())
            {
                values.erase(it);
                return 1;
            }
            else
            {
                cout<<"Cannot remove : This key does not exists"<<endl;
                return 0;
            }
        }

        void search(int key)
        {
            int index= -1;
            for(int i=0; i<values.size(); i++)
            {
                if(values[i]== key)
                    index= i;
            }

            if(index != -1)
            {
                cout<<"Key exists"<<endl;
            }
            else
            {
                cout<<"This key does not exists"<<endl;
            }
        }


        int isEmpty()
        {
            return (values.size()==0);
        }

        vector<int> copy()
        {
            vector<int> temp(values.begin(),values.end());
            return temp;
        }

        void clear()
        {
            values.clear();
        }

        void display()
        {
            for(int i=0; i< values.size();i++)
                cout<<values[i]<<" ";

            cout<<"-- "<<values.size()<<" "<<depth;
            cout<<endl;
        }
};

class Directory {
        int global_depth, bucket_size;
        vector<Bucket*> buckets;

    public:
        Directory(int depth, int bucket_size)
        {
            this->global_depth = depth;
            this->bucket_size = bucket_size;
            for(int i = 0 ; i < pow(2,depth) ; i++ )
            {
                buckets.push_back(new Bucket(depth,bucket_size));
            }
        }

        int hash(int n)
        {
            return n & int(pow(2, global_depth) - 1);
        }

        void search(int key)
        {
            int bucket_no = hash(key);
            cout<<"Searching key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
            buckets[bucket_no]->search(key);
        }

        int pairIndex(int bucket_no, int depth)
        {
            return bucket_no^ int(pow(2,depth-1));
        }

        void grow()
        {
            for(int i = 0 ; i < pow(2, global_depth) ; i++ )
                buckets.push_back(buckets[i]);
            global_depth++;
        }

        void split(int bucket_no)
        {
            //increase depth

            buckets[bucket_no]->depth++;
            int local_depth = buckets[bucket_no]->depth;
            if(local_depth>global_depth)
                grow();

            int pair_index = pairIndex(bucket_no,local_depth);

            buckets[pair_index] = new Bucket(local_depth,bucket_size);

            //clearing the full bucket and reinserting all elements in directory
            vector<int> temp = buckets[bucket_no]->copy();
            buckets[bucket_no]->clear();

            int index_diff = pow(2, local_depth);
            int dir_size = pow(2, global_depth);

            for(int i = pair_index-index_diff ; i>=0 ; i-=index_diff ){
                buckets[i] = buckets[pair_index];
            }

            for(int i = pair_index+index_diff ; i<dir_size ; i+=index_diff ){
                buckets[i] = buckets[pair_index];
            }

            vector<int>::iterator it;
            for(it=temp.begin();it!=temp.end();it++)
                insert(*it,1);
        }

        void insert(int key,bool reinserted)
        {
            int bucket_no = hash(key);
            int status = buckets[bucket_no]->insert(key);
            if(status==1)
            {
                if(!reinserted)
                    cout<<"Inserted key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
                else
                    cout<<"Moved key "<<key<<" to bucket "<<bucket_id(bucket_no)<<endl;
            }
            else if(status==0)
            {
                split(bucket_no);
                insert(key,reinserted);
            }
            else
            {
                cout<<"Key "<<key<<" already exists in bucket "<<bucket_id(bucket_no)<<endl;
            }
        }

        void shrink()
        {
            int i,flag=1;
            for( i=0 ; i<buckets.size() ; i++ )
            {
                if(buckets[i]->depth==global_depth)
                {
                    flag=0;
                    return;
                }
            }
            global_depth--;
            for(i = 0 ; i < pow(2, global_depth) ; i++ )
                buckets.pop_back();
        }

        void merge(int bucket_no)
        {
            int local_depth = buckets[bucket_no]->depth;
            int pair_index = pairIndex(bucket_no,local_depth);
            int index_diff = pow(2, local_depth);
            int dir_size = pow(2, global_depth);

            if( buckets[pair_index]->depth == local_depth )
            {
                buckets[pair_index]->depth--;
                delete(buckets[bucket_no]);

                buckets[bucket_no] = buckets[pair_index];

                for(int i=bucket_no-index_diff ; i>=0 ; i-=index_diff )
                    buckets[i] = buckets[pair_index];

                for(int i=bucket_no+index_diff ; i<dir_size ; i+=index_diff )
                    buckets[i] = buckets[pair_index];
            }
        }


        void remove(int key,int mode)
        {
            int bucket_no = hash(key);
            if(buckets[bucket_no]->remove(key))
                cout<<"Deleted key "<<key<<" from bucket "<<bucket_id(bucket_no)<<endl;
            if(mode>0)
            {
                if(buckets[bucket_no]->isEmpty() && buckets[bucket_no]->depth>1)
                    merge(bucket_no);
            }
            if(mode>1)
            {
                shrink();
            }
        }

        string bucket_id(int n)
        {
            int d = buckets[n]->depth;
            string s = "";
            while(n>0 && d>0)
            {
                s = (n%2==0?"0":"1")+s;
                n/=2;
                d--;
            }
            while(d>0)
            {
                s = "0"+s;
                d--;
            }
            return s;
        }


        void display()
        {
            int i,j,d;
            string s;
            set<string> shown1;
            set<string> shown;
            cout<<"Global depth : "<<global_depth<<endl;
            int count1=0;
            for(i=0;i<buckets.size();i++)
            {
                s = bucket_id(i);
                if(shown1.find(s)==shown1.end())
                {
                    shown1.insert(s);
                    count1++;
                }
            }
            cout<<count1<<endl;
            for(i=0;i<buckets.size();i++)
            {
                d = buckets[i]->depth;
                s = bucket_id(i);
                if(shown.find(s)==shown.end())
                {
                    shown.insert(s);
                    for(j=d;j<=global_depth;j++)
                        cout<<" ";
                    cout<<s<<" => ";
                    buckets[i]->display();
                }
            }
        }

};

/* Main function */
int main()
{
    int bucket_size, initial_global_depth;
    int key;

    cin>>initial_global_depth;
    cin>>bucket_size;

    Directory d(initial_global_depth,bucket_size);

    int choice;

    do
    {
        cin>>choice;
        if(choice==2)
        {
            cin>>key;
            d.insert(key,0);
        }
        else if(choice==4)
        {
            cin>>key;
            d.remove(key,2);
        }
        else if(choice==3)
        {
            cin>>key;
            d.search(key);
        }
        else if(choice==5)
        {
            d.display();
        }
    } while(choice!=6);

    return 0;
}
