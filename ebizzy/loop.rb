sleep=5

module Enumerable
    def sum
      self.inject(0){|accum, i| accum + i }
    end

    def mean
      self.sum/self.length.to_f
    end

    def sample_variance
      m = self.mean
      sum = self.inject(0){|accum, i| accum +(i-m)**2 }
      sum/(self.length - 1).to_f
    end

    def standard_deviation
      return Math.sqrt(self.sample_variance)
    end

end

for v in [0, 2]
  ENV["MALLOC_VRANGE"] = v.to_s
  printf("----- vrange = %d ---------------\n", v)
  for i in [1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48]
#    for i in [12]
    results = []

    9.times {|j|
      ret = `./ebizzy -S #{sleep} -n 512 -t #{i}`
      ret.gsub!(/glibc:.*\n/, "")
      results[j] = ret.to_f
      sleep 0.2
    }

    #drop min-max
      results.sort!
      results.shift(2)
      results.pop(2)

    avg = results.sum / results.size
    std_per = results.standard_deviation / avg * 100
    if std_per > 5
      puts "   redo"
      printf("   threads: %d\n", i)
      printf("   data: %s\n", results.join(" "))
      printf("   avg: %.2f\n", avg)
      printf("   std: %.2f (%.2f%%)\n", results.standard_deviation, std_per)
      redo;
    end

    printf("\n")
    printf("threads: %d\n", i)
    printf("data: %s\n", results.join(" "))
    printf("avg: %.2f\n", avg)
    printf("std: %.2f (%.2f%%)\n", results.standard_deviation, std_per)
    printf("max: %.0f\n", results.max)
    printf("min: %.0f\n", results.min)
  end
end
