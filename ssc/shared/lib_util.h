#ifndef __lib_util_h
#define __lib_util_h

#include <cstdio>
#include <string>
#include <vector>
#include <cassert>

#include <unordered_map>
using std::unordered_map;

#ifdef _MSC_VER
#pragma warning(disable: 4290)  // ignore warning: 'C++ exception specification ignored except to indicate a function is not __declspec(nothrow)'
#endif
/* 

For proper compilation:

Define _WIN32 if on Windows
Define _DEBUG if compile with debugging

*/

#if defined(_DEBUG) && defined(_MSC_VER) && defined(_WIN32) && !defined(_WIN64)
#define VEC_ASSERT(x) {if(!(x)) _asm{int 0x03}}
#else
#define VEC_ASSERT(X) assert(X)
#endif

#define RCINDEX(arr, ncols, r, c) arr[ncols*r+c]

namespace util
{
	const double watt_to_kilowatt = 1. / 1000;
	const double kilowatt_to_watt = 1000;
	const double hour_to_min = 60.;
	const double Celsius_to_Kelvin = 273.15;

	static const int nday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	std::vector< std::string > split( const std::string &str, const std::string &delim, bool ret_empty=false, bool ret_delim=false );
	std::string join( const std::vector< std::string > &list, const std::string &delim );

	size_t replace( std::string &s, const std::string &old_text, const std::string &new_text);

		
	bool to_integer(const std::string &str, int *x);
	bool to_float(const std::string &str, float *x);
	bool to_double(const std::string &str, double *x);
		
	std::string to_string( int x, const char *fmt="%d" );
	std::string to_string( double x, const char *fmt="%lg" );

	std::string lower_case( const std::string &in );
	std::string upper_case( const std::string &in );
	
	std::string read_file( const std::string &file );
	bool read_line( FILE *fp, std::string &text, int prealloc = 256 );
	
	int hours_in_month(int month); /* returns the number of hours in a month, as used in month_of() */
	int hour_of_day(int hour_of_year); /* return the hour of day (0 - 23) given the hour of year (0 - 8759) */
	double percent_of_year(int month, int hours); /* returns the fraction of a year, based on months and hours */
	int month_of(double time); /* hour: 0 = jan 1st 12am-1am, returns 1-12 */
	int day_of_month(int month, double time); /* month: 1-12 time: hours, starting 0=jan 1st 12am, returns 1-nday*/
	void month_hour(int hour_of_year, int & out_month, int & out_hour); /*given the hour of year, return the month, and hour of day*/
	bool weekday(int hour_of_year); /* return true if is a weekday, assuming first hour of year is Monday at 12 am*/

	int schedule_char_to_int( char c );
	std::string schedule_int_to_month( int m );
	bool translate_schedule(int tod[8760], const char *wkday, const char *wkend, int min_val, int max_val);

	bool file_exists( const char *file );
	bool dir_exists( const char *path );
	bool remove_file( const char *path );
	bool mkdir( const char *path, bool make_full = false); 
	std::string path_only( const std::string &path );
	std::string name_only( const std::string &path );
	std::string ext_only( const std::string &path );
	char path_separator();
	std::string get_cwd();
	bool set_cwd( const std::string &path );
	
	class sync_piped_process
	{
	public:
		sync_piped_process() {  }
		virtual ~sync_piped_process() {  }

		int spawn(const std::string &command, const std::string &workdir="");
		virtual void on_stdout(const std::string &line_text) = 0;
	};
	
	
	std::string format(const char *fmt, ...);
	size_t format_vn(char *buffer, int maxlen, const char *fmt, va_list arglist);
		
	class stdfile
	{
	public:
		stdfile() : p(0) {  }
		stdfile( FILE *ff ) : p(ff) { }
		stdfile(const char *file, const char *mode) { p = fopen(file, mode); }
		stdfile( const std::string &file, const char *mode ) { p = fopen(file.c_str(), mode); }
		~stdfile() { close(); }
		bool open(const char *file, const char *mode) { close(); p = fopen(file,mode); return ok(); }
		bool open(const std::string &file, const char *mode) {  return open(file.c_str(), mode); }
		bool ok() { return 0!=p; }
		operator FILE*() const { return p; }
		void close() { if (p) ::fclose(p); p=0; }
		FILE *disown() { FILE *ff = p; p = 0; return ff; }
	private:
		FILE *p;
	};

	template< typename T, size_t n_rows, size_t n_cols >
	class matrix_static_t
	{
	protected:
		T t_array[n_rows][n_cols];
	public:
		matrix_static_t( ) { /* nothing to do */ }
		matrix_static_t( const T &fillval )
		{
			fill( fillval );
		}

		void fill( const T &fillval )
		{
			for( size_t i=0;i<n_rows;i++ )
				for( size_t j=0;j<n_cols;j++ )
					t_array[i][j] = fillval;
		}
		void set_column(const T val, size_t c)
		{
			for (size_t i = 0; i< n_rows;i++)
				t_array[i][c] = val;
		}
		void set_row(const T val, size_t r)
		{
			for (size_t j = 0; j< n_cols; j++)
				t_array[r][j] = val;
		}
		void set_value(const T val, size_t r, size_t c)
		{
			t_array[r][c] = val;
		}
		inline T &at(size_t r, size_t c)
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[r][c];
		}

		inline const T &at(size_t r, size_t c) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[r][c];
		}
		
		inline T &operator()(size_t r, size_t c)
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[r][c];
		}

		inline const T &operator()(size_t r, size_t c) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[r][c];
		}
	};

	template< typename T >
	class matrix_t
	{
	protected:
		T *t_array;
		size_t n_rows, n_cols;
	public:

		matrix_t()
		{
			t_array = new T[1];
			n_rows = n_cols = 1;
		}

		matrix_t( const matrix_t &cc )
		{
			n_rows = n_cols = 0;
			t_array = NULL;
			copy( cc );
		}
		
		matrix_t(size_t len)
		{
			n_rows = n_cols = 0;
			t_array = NULL;
			if (len < 1) len = 1;
			resize( 1, len );
		}

		matrix_t(size_t nr, size_t nc)
		{
			n_rows = n_cols = 0;
			t_array = NULL;
			if (nr < 1) nr = 1;
			if (nc < 1) nc = 1;
			resize(nr,nc);
		}
		
		matrix_t(size_t nr, size_t nc, const T &val)
		{
			n_rows = n_cols = 0;
			t_array = NULL;
			if (nr < 1) nr = 1;
			if (nc < 1) nc = 1;
			resize(nr,nc);
			fill(val);
		}
		matrix_t(size_t nr, size_t nc, const std::vector<T> *val)
		{
			n_rows = n_cols = 0;
			t_array = NULL;
			if (nr < 1) nr = 1;
			if (nc < 1) nc = 1;
			resize(nr, nc);
			size_t ncells = n_rows*n_cols;
			for (size_t i = 0; i<ncells; i++)
				t_array[i] = (*val)[i];
		}


		virtual ~matrix_t()
		{
			if (t_array) delete [] t_array;
		}
		
		void clear()
		{
			if (t_array) delete [] t_array;
			n_rows = n_cols = 1;
			t_array = new T[1];
		}
		
		void copy( const matrix_t &rhs )
		{
			if (this != &rhs)
			{
				resize( rhs.nrows(), rhs.ncols() );
				size_t nn = n_rows*n_cols;
				for (size_t i=0;i<nn;i++)
					t_array[i] = rhs.t_array[i];
			}
		}

		void assign( const T *pvalues, size_t len )
		{
			resize( len );
			if ( n_cols == len && n_rows == 1 )
				for (size_t i=0;i<len;i++)
					t_array[i] = pvalues[i];
		}
		
		void assign( const T *pvalues, size_t nr, size_t nc )
		{
			resize( nr, nc );
			if ( n_rows == nr && n_cols == nc )
			{
				size_t len = nr*nc;
				for (size_t i=0;i<len;i++)
					t_array[i] = pvalues[i];
			}
		}

		matrix_t &operator=(const matrix_t &rhs)
		{
			if ( this != &rhs )
				copy( rhs );

			return *this;
		}
		
		matrix_t &operator=(const T &val)
		{
			resize(1,1);
			t_array[0] = val;
			return *this;
		}
		
		inline operator T()
		{
			return t_array[0];
		}
		
		bool equals( const matrix_t & rhs )
		{
			if (n_rows != rhs.n_rows || n_cols != rhs.n_cols)
				return false;
			
			size_t nn = n_rows*n_cols;
			for (size_t i=0;i<nn;i++)
				if (t_array[i] != rhs.t_array[i])
					return false;
			
			return true;
		}
		
		inline bool is_single()
		{
			return (n_rows == 1 && n_cols == 1);
		}
			
		inline bool is_array()
		{
			return (n_rows == 1);
		}
		
		void fill( const T &val )
		{
			size_t ncells = n_rows*n_cols;
			for (size_t i=0;i<ncells;i++)
				t_array[i] = val;
		}
		void resize(size_t nr, size_t nc)
		{
			if (nr < 1 || nc < 1) return;
			if (nr == n_rows && nc == n_cols) return;
			
			if (t_array) delete [] t_array;
			t_array = new T[ nr * nc ];
			n_rows = nr;
			n_cols = nc;
		}

		void resize_fill(size_t nr, size_t nc, const T &val)
		{
			resize( nr, nc );
			fill( val );
		}
		
		void resize(size_t len)
		{
			resize( 1, len );
		}
		
		void resize_fill(size_t len, const T &val)
		{
			resize_fill( 1, len, val );
		}
		void set_value(const T &val, size_t r, size_t c)
		{
			t_array[n_cols*r + c] = val;
		}
		inline T &at(size_t i)
		{
	#ifdef _DEBUG
			VEC_ASSERT( i >= 0 && i < n_cols );
	#endif
			return t_array[i];
		}

		inline const T&at(size_t i) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( i >= 0 && i < n_cols );
	#endif
			return t_array[i];
		}
		
		inline T &at(size_t r, size_t c)
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[n_cols*r+c];
		}

		inline const T &at(size_t r, size_t c) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[n_cols*r+c];
		}
		
		inline T &operator()(size_t r, size_t c)
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[n_cols*r+c];
		}

		inline const T &operator()(size_t r, size_t c) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols );
	#endif
			return t_array[n_cols*r+c];
		}
		
		T operator[] (size_t i) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( i >= 0 && i < n_cols );
	#endif
			return t_array[i];
		}
		
		T &operator[] (size_t i)
		{
	#ifdef _DEBUG
			VEC_ASSERT( i >= 0 && i < n_cols );
	#endif
			return t_array[i];
		}
				
		inline size_t nrows() const
		{
			return n_rows;
		}
		
		inline size_t ncols() const
		{
			return n_cols;
		}
		
		inline size_t ncells() const
		{
			return n_rows*n_cols;
		}
		
		inline size_t membytes() const
		{
			return n_rows*n_cols*sizeof(T);
		}
		
		void size(size_t &nr, size_t &nc) const
		{
			nr = n_rows;
			nc = n_cols;
		}
		
		size_t length() const
		{
			return n_cols;
		}
		
		inline T *data()
		{
			return t_array;
		}

		inline T value() const
		{
			return t_array[0];
		}
	};

	template< typename T >
	class block_t
	{
	private:
		T *t_array;
		size_t n_rows, n_cols, n_layers;
	public:

		block_t()
		{
			t_array = new T[1];
			n_rows = n_cols = n_layers = 1;
		}
		
		block_t(size_t nr, size_t nc, size_t nl)
		{
			t_array = NULL;
			if (nl < 1) nl = 1;
			if (nr < 1) nr = 1;
			if (nc < 1) nc = 1;
			resize(nr,nc,nl);			
		}

		block_t(size_t nr, size_t nc, size_t nl, const T &val)
		{
			t_array = NULL;
			if (nr < 1) nr = 1;
			if (nc < 1) nc = 1;
			if (nl < 1) nl = 1;
			resize(nr,nc,nl);
			fill(val);
		}


		virtual ~block_t()
		{
			if (t_array) delete [] t_array;
		}
		
		void clear()
		{	//Note: when Clear() is called before resize() or resize_fill(), it can cause a memory error.
			//Do not use clear before calling these functions.
			if (t_array) delete [] t_array;
			n_layers = n_rows = n_cols = 0;
		}
		
		void copy( const block_t &rhs )
		{
			if (this != &rhs)
			{
				resize(rhs.nrows(), rhs.ncols(), rhs.nlayers() );
				size_t nn = n_layers*n_rows*n_cols;
				for (size_t i=0;i<nn;i++)
					t_array[i] = rhs.t_array[i];
			}
		}

		void assign( const T *pvalues, size_t nr, size_t nc, size_t nl )
		{
			resize( nr, nc, nl );
			if ( n_rows == nr && n_cols == nc && n_layers == nl)
			{
				size_t len = nr*nc*nl;
				for (size_t i=0;i<len;i++)
					t_array[i] = pvalues[i];
			}
		}

		block_t &operator=(const block_t &rhs)
		{
			copy( rhs );
			return *this;
		}
		
		block_t &operator=(const T &val)
		{
			resize(1,1,1);
			t_array[0] = val;
			return *this;
		}
		
		inline operator T()
		{
			return t_array[0];
		}
		
		bool equals( const block_t & rhs )
		{
			if (n_rows != rhs.n_rows || n_cols != rhs.n_cols || n_layers != rhs.n_layers)
				return false;
			
			size_t nn = n_rows*n_cols*n_layers;
			for (size_t i=0;i<nn;i++)
				if (t_array[i] != rhs.t_array[i])
					return false;
			
			return true;
		}
		
		inline bool is_single()
		{
			return (n_rows == 1 && n_cols == 1 && n_layers == 1);
		}
			
		inline bool is_array()
		{
			return (n_rows == 1 && n_layers == 1);
		}
		
		void fill( const T &val )
		{
			size_t ncells = n_rows*n_cols*n_layers;
			for (size_t i=0;i<ncells;i++)
				t_array[i] = val;
		}

		void resize(size_t nr, size_t nc, size_t nl)
		{
			if (nr < 1 || nc < 1 || nl < 1) return;
			if (nr == n_rows && nc == n_cols && n_layers) return;
			
			if (t_array) delete [] t_array;
			t_array = new T[ nr * nc * nl];
			n_rows = nr;
			n_cols = nc;
			n_layers = nl;
		}

		void resize_fill(size_t nr, size_t nc, size_t nl, const T &val)
		{
			resize( nr, nc, nl);
			fill( val );
		}
		
		void resize(size_t len)
		{
			resize( 1, len, 1);
		}
		
		void resize_fill(size_t len, const T &val)
		{
			resize_fill( 1, len, 1, val );
		}
		
		inline T &at(size_t r, size_t c, size_t l)
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols && l >= 0 && l < n_layers);
	#endif
			return t_array[n_cols*(n_rows*l + r)+c];
		}

		inline const T &at(size_t r, size_t c, size_t l) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( r >= 0 && r < n_rows && c >= 0 && c < n_cols && l >= 0 && l < n_layers);
	#endif
			return t_array[n_cols*(n_rows*l + r)+c];
		}
		
		T operator[] (size_t i) const
		{
	#ifdef _DEBUG
			VEC_ASSERT( i >= 0 && i < n_cols );
	#endif
			return t_array[i];
		}
		
		T &operator[] (size_t i)
		{
	#ifdef _DEBUG
			VEC_ASSERT( i >= 0 && i < n_cols );
	#endif
			return t_array[i];
		}
				
		inline size_t nrows() const
		{
			return n_rows;
		}
		
		inline size_t ncols() const
		{
			return n_cols;
		}
		
		inline size_t nlayers() const
		{
			return n_layers;
		}
		
		inline size_t ncells() const
		{
			return n_rows*n_cols*n_layers;
		}
		
		inline size_t membytes() const
		{
			return n_rows*n_cols*n_layers*sizeof(T);
		}
		
		void size(size_t &nr, size_t &nc, size_t &nl) const
		{
			nr = n_rows;
			nc = n_cols;
			nl = n_layers;
		}
		
		size_t length() const
		{
			return n_cols;
		}
		
		inline T *data()
		{
			return t_array;
		}

		inline T value() const
		{
			return t_array[0];
		}
	};

	double bilinear( double rowval, double colval, const matrix_t<double> &mat );
	double interpolate(double x1, double y1, double x2, double y2, double xValueToGetYValueFor);
	double linterp_col( const matrix_t<double> &mat, size_t ixcol, double xval, size_t iycol );
	bool translate_schedule(int tod[8760], const matrix_t<float> &wkday, const matrix_t<float> &wkend, int min_val, int max_val);
};


#endif