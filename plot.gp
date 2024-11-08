# Set up the terminal for PNG output
set terminal png size 800,600
set output 'plots/stock_price_variation.png'

# Set title and labels
set title "Stock Price Variation"
set xlabel "Timestamp (in days)"
set ylabel "Price ($)"

# Set Y-axis range from 0 to 500
set yrange [0:500]

# Plot using line numbers from 100 to 200
plot 'data/stock_prices.dat' every ::100 using ($1/100000000):3 with linespoints title 'AAPL' linecolor rgb 'blue'
