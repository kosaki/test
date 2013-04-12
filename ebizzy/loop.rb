sleep=5

for i in [1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48]

  ENV["MALLOC_VRANGE"]=0.to_s
  result = `./ebizzy -S #{sleep} -t #{i}`
  ENV["MALLOC_VRANGE"]=1.to_s
  result2 = `./ebizzy -S #{sleep} -t #{i}`
  result2.gsub!("glibc: vrange enabled\n", "")
  printf("%3d %8d %8d\n", i, result, result2);

#  ENV["MALLOC_VRANGE"]=1.to_s
#  system("./ebizzy -S #{sleep} -t #{i}")
end

#puts("---")
#puts("vrange")
#puts("---")


#for i in [1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48]
#  system("./ebizzy -S #{sleep} -t #{i}")
#end
