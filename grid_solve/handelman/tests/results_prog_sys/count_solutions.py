import csv
import sys
N=0
total_dict={}
free_vars=0

def make_dict(row,cnt):
    ans={}
    
    if(cnt==0):
        return 1
    if(cnt==1):
        for j in range(int(row[0]),int(row[1])+1):
            ans[j]=1
    else:
        for j in range(int(row[0]),int(row[1])+1):
                ans[j]=make_dict(row[2:],cnt-1)
    return ans

def count_max_solns(curr_dict,dep):
    
    global free_vars,N

    

    ans=0
  
   
    for key in curr_dict.keys():
        if(dep==1):
          
            ans+=count_dict(curr_dict[key],N-free_vars)
        else:
            ans=max(ans,count_max_solns(curr_dict[key],dep-1))
    
    return ans





def add_pt(row,cnt,curr_dict):
    if(cnt==0):
        return curr_dict
    for j in range(int(row[0]),int(row[1])+1):
        if(j in curr_dict.keys()):
            curr_dict[j]=add_pt(row[2:],cnt-1,curr_dict[j])
        else:
            curr_dict[j]=make_dict(row[2:],cnt-1)
    return curr_dict

def count_dict(dict_,N):
 
    if(N==0):
        return 0
    elif(N==1):
        return len(dict_.keys())
    else:
        ans=0
        for key in dict_.keys():
            ans+=count_dict(dict_[key],N-1)
        return ans

def process_row(row):
    # Define your processing function here
    # For example, let's just print the row
    global N,total_dict
    
    total_dict=add_pt(row,N,total_dict)







def main():
    # Take integer input from the user
    global N,total_dict,free_vars
    i = int(sys.argv[1])

    # Construct the filename based on user input
    filename = f"results{i}-1.csv"

    # Read the CSV file
    with open(filename, 'r') as file:
        csv_reader = csv.reader(file)
        
        # Read the header to get the number of columns
        header = next(csv_reader)
        num_columns = len(header)
        if(N==0):
          N=(num_columns-1)//2
        print(f"Number of columns: {num_columns}")

        # Process each row
        for row in csv_reader:
            process_row(row)
    
    ans=count_dict(total_dict,N)

    free_vars=int(sys.argv[2])


    print(f"Number of solutions: {ans}")

    print(f"Max progs for one input: {count_max_solns(total_dict,free_vars)}")

if __name__ == "__main__":
    main()