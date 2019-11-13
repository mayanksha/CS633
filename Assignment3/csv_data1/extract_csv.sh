make
folder_path="./data1"
out_folder_path="./csv_data1"

files=($(echo $(ls ${folder_path})))

for i in "${files[@]}"
do
    mpiexec ./src.x "${folder_path}/${i}" > "${out_folder_path}/${i}.csv"
done
