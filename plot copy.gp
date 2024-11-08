set terminal png size 800,600
set output 'plots/aapl_stock_prices.png'  # Save to plots directory
set title "AAPL Stock Prices Over Time"
set xlabel "Time (s)"
set ylabel "Price ($)"
set grid
set xdata time
set timefmt "%s"
set xrange [*:*]  # Automatically adjust the x-axis range
plot 'data/sorted_AAPL_stock_prices.dat' using 1:3 with lines title 'AAPL' linecolor rgb 'blue'
